typedef struct R_Parser
{
	R_Arena* ast_arena;
	R_Lexer lexer;
	R_Token current;
} R_Parser;

R_Token
R_Parser_GetToken(R_Parser* state)
{
	return state->current;
}

R_Token
R_Parser_NextToken(R_Parser* state)
{
	state->current = R_Lexer_NextToken(&state->lexer);
	return state->current;
}

R_bool
R_Parser_IsToken(R_Parser* state, R_Token_Kind kind)
{
	return (state->current.kind == kind);
}

R_bool
R_Parser_EatToken(R_Parser* state, R_Token_Kind kind)
{
	if (state->current.kind == kind)
	{
		R_Parser_NextToken(state);
		return R_true;
	}
	else return R_false;
}

#define R_GetToken()  R_Parser_GetToken(state)
#define R_NextToken() R_Parser_NextToken(state)
#define R_IsToken(k)  R_Parser_IsToken(state, (k))
#define R_EatToken(k) R_Parser_EatToken(state, (k))

// TODO: Maybe reduce footprint
void*
R_Parser_PushNode(R_Parser* state, R_AST_Kind kind)
{
	R_AST* node = R_Arena_Push(state->ast_arena, sizeof(R_AST), R_ALIGNOF(R_AST));
	node->kind = kind;
	node->next = 0;
	return node;
}

R_bool R_Parser_ParseExpression(R_Parser* state, R_Expression** expr);
R_bool R_Parser_ParseStatement (R_Parser* state, R_Statement** stmnt);

R_bool
R_Parser_ParseExpressionList(R_Parser* state, R_Expression** expr)
{
	for (;;)
	{
		if (!R_Parser_ParseExpression(state, expr)) return R_false;
		else
		{
			if (!R_EatToken(R_Token_Comma)) break;
			else
			{
				expr = &(*expr)->next;
				continue;
			}
		}
	}

	return R_true;
}

R_bool
R_Parser_ParseArgumentList(R_Parser* state, R_Argument** args)
{
	R_Argument** next_arg = args;
	for (;;)
	{
		R_Expression* name = 0;
		R_Expression* value;
		if (!R_Parser_ParseExpression(state, &value)) return R_false;
		else
		{
			if (R_EatToken(R_Token_Equals))
			{
				name = value;
				if (!R_Parser_ParseExpression(state, &value)) return R_false;
			}

			*next_arg = R_Parser_PushNode(state, R_AST_Argument);
			(*next_arg)->name  = name;
			(*next_arg)->value = value;

			next_arg = &(*next_arg)->next;

			if (R_EatToken(R_Token_Comma)) continue;
			else                           break;
		}
	}
	return R_true;
}

R_bool
R_Parser_ParsePrimaryExpression(R_Parser* state, R_Expression** expr)
{
	R_Token token = R_GetToken();

	if (token.kind == R_Token_Identifier)
	{
		*expr = R_Parser_PushNode(state, R_AST_String);
		(*expr)->identifier = token.string;
	}
	else if (token.kind == R_Token_String)
	{
		*expr = R_Parser_PushNode(state, R_AST_String);
		(*expr)->string = token.string;
	}
	else if (token.kind >= R_Token__FirstInteger && token.kind <= R_Token__LastInteger)
	{
		*expr = R_Parser_PushNode(state, R_AST_Int);
		(*expr)->integer = token.integer;
	}
	else if (token.kind >= R_Token__FirstFloat && token.kind <= R_Token__LastFloat)
	{
		*expr = R_Parser_PushNode(state, R_AST_Float);
		(*expr)->floating = token.floating;
	}
	else if (token.kind == R_Token_True || token.kind == R_Token_False)
	{
		R_bool boolean = (token.kind = R_Token_True);

		*expr = R_Parser_PushNode(state, R_AST_Bool);
		(*expr)->boolean = boolean;
	}
	else if (token.kind == R_Token_Proc)
	{
		R_Parameter* params       = 0;
		R_Expression* return_type = 0;
		R_Statement* body         = 0;

		if (R_EatToken(R_Token_OpenParen))
		{
			R_Expression* expressions;
			if (!R_Parser_ParseExpressionList(state, &expressions)) return R_false;
			else
			{
				if (!R_IsToken(R_Token_Colon) && !R_IsToken(R_Token_Equals))
				{
					R_Parameter** next_param = &params;
					for (R_Expression* scan = expressions; scan != 0; )
					{
						R_Expression* next_scan = scan->next;

						R_Expression* type = scan;
						type->next = 0;

						*next_param = R_Parser_PushNode(state, R_AST_Parameter);
						(*next_param)->names = 0;
						(*next_param)->type  = type;
						(*next_param)->value = 0;

						next_param = &(*next_param)->next;
						scan       = next_scan;
					}
				}
				else
				{
					R_Expression* names      = expressions;
					R_Parameter** next_param = &params;
					for (;;)
					{
						R_Expression* type  = 0;
						R_Expression* value = 0;

						if (!R_EatToken(R_Token_Colon))
						{
							//// ERROR: Missing colon separator between names and type of parameters
							R_NOT_IMPLEMENTED;
							return R_false;
						}
						else
						{
							if (!R_IsToken(R_Token_Equals))
							{
								if (!R_Parser_ParseExpression(state, &type)) return R_false;
							}

							if (R_EatToken(R_Token_Equals))
							{
								if (!R_Parser_ParseExpression(state, &value)) return R_false;
							}

							*next_param = R_Parser_PushNode(state, R_AST_Parameter);
							(*next_param)->names = names;
							(*next_param)->type  = type;
							(*next_param)->value = value;

							next_param = &(*next_param)->next;

							if (!R_EatToken(R_Token_Comma)) break;
							else
							{
								if (!R_Parser_ParseExpressionList(state, &names)) return R_false;
								else                                              continue;
							}
						}
					}
				}
			}

			if (!R_EatToken(R_Token_CloseParen))
			{
				//// ERROR: Missing closing parenthesis after paramter list
				R_NOT_IMPLEMENTED;
				return R_false;
			}
		}

		if (R_EatToken(R_Token_Arrow))
		{
			if (!R_Parser_ParseExpression(state, &return_type))
			{
				return R_false;
			}
		}

		if (!R_IsToken(R_Token_OpenBrace))
		{
			*expr = R_Parser_PushNode(state, R_AST_ProcType);
			(*expr)->proc_type_expr.params      = params;
			(*expr)->proc_type_expr.return_type = return_type;
		}
		else
		{
			if (!R_Parser_ParseStatement(state, &body)) return R_false;
			else
			{
				*expr = R_Parser_PushNode(state, R_AST_ProcLit);
				(*expr)->proc_lit_expr.params      = params;
				(*expr)->proc_lit_expr.return_type = return_type;
				(*expr)->proc_lit_expr.body        = body;
			}
		}
	}
	else if (R_EatToken(R_Token_Struct))
	{
		if (!R_EatToken(R_Token_OpenBrace))
		{
			//// ERROR: Missing open brace before body of struct
			R_NOT_IMPLEMENTED;
			return R_false;
		}
		else
		{
			R_Declaration* members = 0;

			R_Declaration** next_member = &members;
			for (;;)
			{
				if      (R_EatToken(R_Token_CloseBrace)) break;
				else if (R_IsToken(R_Token_EndOfFile))
				{
					//// ERROR: Reached end of file before terminating close brace for struct body
					R_NOT_IMPLEMENTED;
					return R_false;
				}
				else
				{
					R_Statement* stmnt;
					if (!R_Parser_ParseStatement(state, &stmnt)) return R_false;
					else
					{
						if (stmnt->kind < R_AST__FirstDeclaration || stmnt->kind > R_AST__LastDeclaration)
						{
							//// ERROR: Struct body may only contain declarations
							R_NOT_IMPLEMENTED;
							return R_false;
						}
						else
						{
							*next_member = (R_Declaration*)stmnt;

							next_member = &(*next_member)->next;
						}
					}
				}
			}

			*expr = R_Parser_PushNode(state, R_AST_StructType);
			(*expr)->struct_type_expr.members = members;
		}
	}
	else if (R_EatToken(R_Token_OpenParen))
	{
		R_Expression* inner_expr;
		if (!R_Parser_ParseExpression(state, &inner_expr)) return R_false;
		else
		{
			*expr = R_Parser_PushNode(state, R_AST_Compound);
			(*expr)->compound_expr.expr = inner_expr;
		}
	}
	else
	{
		if (token.kind == R_Token_Invalid)
		{
			//// ERROR: Lexer error
			R_NOT_IMPLEMENTED;
			return R_false;
		}
		else
		{
			//// ERROR: Missing primary expression
			R_NOT_IMPLEMENTED;
			return R_false;
		}
	}

	return R_true;
}

R_bool
R_Parser_ParsePostfixExpression(R_Parser* state, R_Expression** expr)
{
	if (!R_Parser_ParsePrimaryExpression(state, expr)) return R_false;
	else
	{
		for (;;)
		{
			if (R_EatToken(R_Token_OpenBracket))
			{
				R_Expression* first;
				if (!R_Parser_ParseExpression(state, &first)) return R_false;
				else
				{
					if (R_EatToken(R_Token_CloseBracket))
					{
						R_Expression* array = *expr;
						R_Expression* index = first;

						*expr = R_Parser_PushNode(state, R_AST_Subscript);
						(*expr)->subscript_expr.array = array;
						(*expr)->subscript_expr.index = index;
					}
					else
					{
						if (!R_EatToken(R_Token_Colon))
						{
							//// ERROR: Either missing closing bracket after subscript in subscript expression or colon after start index in slice expression
							R_NOT_IMPLEMENTED;
							return R_false;
						}
						else
						{
							R_Expression* array = *expr;
							R_Expression* start = first;
							R_Expression* past_end;
							if (!R_Parser_ParseExpression(state, &past_end)) return R_false;
							else if (!R_EatToken(R_Token_CloseBracket))
							{
								//// ERROR: Missing closing bracket after slice expression
								R_NOT_IMPLEMENTED;
								return R_false;
							}
							else
							{
								*expr = R_Parser_PushNode(state, R_AST_Slice);
								(*expr)->slice_expr.array    = array;
								(*expr)->slice_expr.start    = start;
								(*expr)->slice_expr.past_end = past_end;
							}
						}
					}
				}
			}
			else if (R_EatToken(R_Token_Period))
			{
				R_Expression* namespace = *expr;
				R_Expression* member;
				if (R_Parser_ParsePrimaryExpression(state, &member)) return R_false;
				else
				{
					*expr = R_Parser_PushNode(state, R_AST_Member);
					(*expr)->member_expr.namespace = namespace;
					(*expr)->member_expr.member    = member;
				}
			}
			else if (R_EatToken(R_Token_Hat))
			{
				R_Expression* operand = *expr;

				*expr = R_Parser_PushNode(state, R_AST_Dereference);
				(*expr)->unary_expr.operand = operand;
			}
			else if (R_EatToken(R_Token_OpenParen))
			{
				R_Expression* proc = *expr;
				R_Argument* args;
				if (!R_Parser_ParseArgumentList(state, &args)) return R_false;
				else
				{
					if (!R_EatToken(R_Token_CloseParen))
					{
						//// ERROR: Missing closing parenthesis after arguments to procedure call
						R_NOT_IMPLEMENTED;
						return R_false;
					}
					else
					{
						*expr = R_Parser_PushNode(state, R_AST_Call);
						(*expr)->call_expr.proc = proc;
						(*expr)->call_expr.args = args;
					}
				}
			}
			else break;
		}
	}

	return R_true;
}

R_bool
R_Parser_ParseTypePrefixExpression(R_Parser* state, R_Expression** expr)
{
	for (;;)
	{
		if (R_EatToken(R_Token_Hat))
		{
			*expr = R_Parser_PushNode(state, R_AST_PointerType);

			expr = &(*expr)->unary_expr.operand;
		}
		else if (R_EatToken(R_Token_OpenBracket))
		{
			if (R_EatToken(R_Token_CloseBracket))
			{
				*expr = R_Parser_PushNode(state, R_AST_SliceType);

				expr = &(*expr)->unary_expr.operand;
			}
			else
			{
				R_Expression* size;
				if      (!R_Parser_ParseExpression(state, &size)) return R_false;
				else if (!R_EatToken(R_Token_CloseBracket))
				{
					//// ERROR: Missing closing bracket after size in array type prefix
					R_NOT_IMPLEMENTED;
					return R_false;
				}
				else
				{
					*expr = R_Parser_PushNode(state, R_AST_ArrayType);
					(*expr)->array_type_expr.size = size;

					expr = &(*expr)->array_type_expr.type;
				}
			}
		}
		else return !R_Parser_ParsePostfixExpression(state, expr);
	}
}

R_bool
R_Parser_ParseUnaryExpression(R_Parser* state, R_Expression** expr)
{
	for (;;)
	{
		R_Token token = R_GetToken();

		R_AST_Kind op = R_AST_None;
		switch (token.kind)
		{
			case R_Token_Add:   op = R_AST_Pos;       break;
			case R_Token_Sub:   op = R_AST_Neg;       break;
			case R_Token_Bang:  op = R_AST_Not;       break;
			case R_Token_Tilde: op = R_AST_BitNot;    break;
			case R_Token_And:   op = R_AST_Reference; break;
		}

		if (op == R_AST_None) return !R_Parser_ParseTypePrefixExpression(state, expr);
		else
		{
			*expr = R_Parser_PushNode(state, op);
			expr = &(*expr)->unary_expr.operand;
		}
	}
}

R_bool
R_Parser_ParseBinaryExpression(R_Parser* state, R_Expression** expr)
{
	if (R_Parser_ParseUnaryExpression(state, expr)) return R_false;
	else
	{
		for (;;)
		{
			R_Token token = R_GetToken();

			if (token.kind >= R_Token__FirstBinary && token.kind <= R_Token__LastBinary)
			{
				R_AST_Kind op = (R_AST_Kind)token.kind;
				R_NextToken();

				R_umm precedence = R_AST_BLOCK_INDEX_FROM_KIND(op);

				R_Expression** slot = expr;
				R_Expression* right;
				if (R_Parser_ParseUnaryExpression(state, &right)) return R_false;
				else
				{
					for (;;)
					{
						if (R_AST_BLOCK_INDEX_FROM_KIND((*slot)->kind) > precedence)
						{
							R_ASSERT(R_AST_IS_BINARY_EXPRESSION((*slot)->kind));
							slot = &(*slot)->binary_expr.right;
							continue;
						}
						else
						{
							R_Expression* left = *slot;

							*slot = R_Parser_PushNode(state, op);
							(*slot)->binary_expr.left  = left;
							(*slot)->binary_expr.right = right;
							break;
						}
					}
				}
			}
		}
	}

	return R_true;
}

R_bool
R_Parser_ParseConditionalExpression(R_Parser* state, R_Expression** expr)
{
	if (!R_Parser_ParseBinaryExpression(state, expr)) return R_false;
	else if (R_EatToken(R_Token_Qmark))
	{
		R_Expression* condition = *expr;
		R_Expression* then_expr;
		R_Expression* else_expr;

		if (!R_Parser_ParseBinaryExpression(state, &then_expr)) return R_false;
		else
		{
			if (!R_EatToken(R_Token_Colon))
			{
				//// ERROR: Missing colon after then part in conditional expression
				R_NOT_IMPLEMENTED;
				return R_false;
			}
			else if (R_Parser_ParseBinaryExpression(state, &else_expr)) return R_false;
			else
			{
				*expr = R_Parser_PushNode(state, R_AST_Conditional);
				(*expr)->conditional_expr.condition = condition;
				(*expr)->conditional_expr.then_expr = then_expr;
				(*expr)->conditional_expr.else_expr = else_expr;
			}
		}
	}

	return R_true;
}

R_bool
R_Parser_ParseExpression(R_Parser* state, R_Expression** expr)
{
	return R_Parser_ParseConditionalExpression(state, expr);
}

R_bool
R_Parser_ParseStatement(R_Parser* state, R_Statement** stmnt)
{
	if (R_IsToken(R_Token_Semicolon))
	{
		//// ERROR: Stray semicolon
		R_NOT_IMPLEMENTED;
		return R_false;
	}
	else if (R_EatToken(R_Token_OpenBrace))
	{
		R_Statement* body        = 0;
		R_Statement** next_stmnt = &body;
		for (;;)
		{
			if      (R_EatToken(R_Token_CloseBrace)) break;
			else if (R_IsToken(R_Token_EndOfFile))
			{
				//// ERROR: Encountered end of file before terminating closing brace
				R_NOT_IMPLEMENTED;
				return R_false;
			}
			else
			{
				if (!R_Parser_ParseStatement(state, next_stmnt)) return R_false;
				else
				{
					next_stmnt = &(*next_stmnt)->next;
					continue;
				}
			}
		}

		*stmnt = R_Parser_PushNode(state, R_AST_Block);
		(*stmnt)->block_stmnt.body = body;
	}
	else if (R_EatToken(R_Token_If))
	{
		if (!R_EatToken(R_Token_OpenParen))
		{
			//// ERROR: Missing open parenthesis before if condition
			R_NOT_IMPLEMENTED;
			return R_false;
		}
		else
		{
			R_Expression* condition;
			R_Statement* then_stmnt;
			R_Statement* else_stmnt = 0;
			if      (!R_Parser_ParseExpression(state, &condition)) return R_false;
			else if (!R_EatToken(R_Token_CloseParen))
			{
				//// ERROR: Missing closing parenthesis after if condition
				R_NOT_IMPLEMENTED;
				return R_false;
			}
			else
			{
				if      (!R_Parser_ParseStatement(state, &then_stmnt))                             return R_false;
				else if (R_EatToken(R_Token_Else) && !R_Parser_ParseStatement(state, &else_stmnt)) return R_false;
				else
				{
					*stmnt = R_Parser_PushNode(state, R_AST_If);
					(*stmnt)->if_stmnt.condition  = condition;
					(*stmnt)->if_stmnt.then_stmnt = then_stmnt;
					(*stmnt)->if_stmnt.else_stmnt = else_stmnt;
				}
			}
		}
	}
	else if (R_IsToken(R_Token_Else))
	{
		//// ERROR: Else without matching if
		R_NOT_IMPLEMENTED;
		return R_false;
	}
	else if (R_EatToken(R_Token_While))
	{
		if (!R_EatToken(R_Token_OpenParen))
		{
			//// ERROR: Missing open parenthesis before while condition
			R_NOT_IMPLEMENTED;
			return  R_false;
		}
		else
		{
			R_Expression* condition;
			R_Statement* body;
			if      (!R_Parser_ParseExpression(state, &condition)) return R_false;
			else if (!R_EatToken(R_Token_CloseParen))
			{
				//// ERROR: Missing closing parenthesis after while condition
				R_NOT_IMPLEMENTED;
				return R_false;
			}
			else
			{
				if (!R_Parser_ParseStatement(state, &body)) return R_false;
				else
				{
					*stmnt = R_Parser_PushNode(state, R_AST_While);
					(*stmnt)->while_stmnt.condition = condition;
					(*stmnt)->while_stmnt.body      = body;
				}
			}
		}
	}
	else if (R_EatToken(R_Token_Break))
	{
		if (!R_EatToken(R_Token_Semicolon))
		{
			//// ERROR: Missing semicolon after break statement
			R_NOT_IMPLEMENTED;
			return R_false;
		}
		else
		{
			*stmnt = R_Parser_PushNode(state, R_AST_Break);
		}
	}
	else if (R_EatToken(R_Token_Continue))
	{
		if (!R_EatToken(R_Token_Semicolon))
		{
			//// ERROR: Missing semicolon after continue statement
			R_NOT_IMPLEMENTED;
			return R_false;
		}
		else
		{
			*stmnt = R_Parser_PushNode(state, R_AST_Continue);
		}
	}
	else if (R_EatToken(R_Token_Return))
	{
		R_Expression* values = 0;
		if (!R_IsToken(R_Token_Semicolon))
		{
			if (!R_Parser_ParseExpressionList(state, &values)) return R_false;
		}

		if (!R_EatToken(R_Token_Semicolon))
		{
			//// ERROR: Missing semicolon after return statement
			R_NOT_IMPLEMENTED;
			return R_false;
		}

		*stmnt = R_Parser_PushNode(state, R_AST_Return);
		(*stmnt)->return_stmnt.values = values;
	}
	else
	{
		R_Expression* expressions;
		if (!R_Parser_ParseExpressionList(state, &expressions)) return R_false;
		else
		{
			if (R_EatToken(R_Token_Colon))
			{
				R_Expression* names = expressions;
				R_Expression* type  = 0;

				if (!R_IsToken(R_Token_Equals) && !R_IsToken(R_Token_Colon))
				{
					if (!R_Parser_ParseExpression(state, &type)) return R_false;
				}

				if (R_EatToken(R_Token_Colon))
				{
					R_Expression* values;
					if (!R_Parser_ParseExpressionList(state, &values)) return R_false;
					else
					{
						if (values->next == 0 && (values->kind == R_AST_ProcLit || values->kind == R_AST_StructType))
						{
							if (R_IsToken(R_Token_Semicolon))
							{
								//// ERROR: Single constant declaration of procedure or struct literal may not be terminated by a semicolon
								R_NOT_IMPLEMENTED;
								return R_false;
							}
						}
						else
						{
							if (!R_EatToken(R_Token_Semicolon))
							{
								//// ERROR: Missing terminating semicolon after constant declaration
								R_NOT_IMPLEMENTED;
								return R_false;
							}
						}

						*stmnt = R_Parser_PushNode(state, R_AST_Constant);
						(*stmnt)->declaration.const_decl.names  = names;
						(*stmnt)->declaration.const_decl.type   = type;
						(*stmnt)->declaration.const_decl.values = values;
					}
				}
				else
				{
					R_Expression* values = 0;

					if (R_EatToken(R_Token_Equals))
					{
						if (!R_Parser_ParseExpressionList(state, &values)) return R_false;
					}

					if (!R_EatToken(R_Token_Semicolon))
					{
						//// ERROR: Missing terminating semicolon after variable declaration
						R_NOT_IMPLEMENTED;
						return R_false;
					}
					else
					{
						*stmnt = R_Parser_PushNode(state, R_AST_Variable);
						(*stmnt)->declaration.var_decl.names  = names;
						(*stmnt)->declaration.var_decl.type   = type;
						(*stmnt)->declaration.var_decl.values = values;
					}
				}
			}
			else
			{
				R_Token token = R_GetToken();
				if (token.kind >= R_Token__FirstAssignment && token.kind <= R_Token__LastAssignment)
				{
					R_Expression* lhs = expressions;
					R_Expression* rhs;
					R_AST_Kind op = (token.kind == R_Token_Equals ? R_AST_None : R_TOKEN_OP_OF_BINARY_OP_ASSIGNMENT(token.kind));
					R_NextToken();

					if (!R_Parser_ParseExpressionList(state, &rhs)) return R_false;
					else
					{
						*stmnt = R_Parser_PushNode(state, R_AST_Assignment);
						(*stmnt)->assign_stmnt.lhs = lhs;
						(*stmnt)->assign_stmnt.rhs = rhs;
						(*stmnt)->assign_stmnt.op  = op;
					}
				}
				else
				{
					if (expressions->next != 0)
					{
						//// ERROR: Stray expression list
						R_NOT_IMPLEMENTED;
						return R_false;
					}
					else
					{
						*stmnt = (R_Statement*)expressions;
					}
				}
			}
		}
	}

	return R_true;
}

R_bool
R_Parser_ParseFile(R_ZString string, R_Arena* ast_arena, R_Arena* string_arena, R_Declaration** ast)
{
	R_Parser* state = &(R_Parser){0};
	state->ast_arena = ast_arena;
	state->lexer     = R_Lexer_Init(string, string_arena);
	state->current   = R_Lexer_NextToken(&state->lexer);

	R_Declaration** next_decl = ast;
	for (;;)
	{
		if (R_IsToken(R_Token_EndOfFile)) break;
		else
		{
			R_Statement* stmnt;
			if (!R_Parser_ParseStatement(state, &stmnt)) return R_false;
			else if (stmnt->kind < R_AST__FirstDeclaration || stmnt->kind > R_AST__LastDeclaration)
			{
				//// ERROR: Only declarations are allowed in file scope
				R_NOT_IMPLEMENTED;
				return R_true;
			}
			else
			{
				*next_decl = (R_Declaration*)stmnt;

				next_decl = &(*next_decl)->next;
			}
		}
	}

	return R_true;
}

#undef R_GetToken
#undef R_NextToken
#undef R_IsToken
#undef R_EatToken
