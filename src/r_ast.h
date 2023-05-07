#define R_AST_BLOCK_SIZE 16
#define R_AST_BLOCK_INDEX_FROM_KIND(kind) ((kind) >> 4)
#define R_AST_BLOCK_OFFSET_FROM_KIND(kind) ((kind) & 0xF)
#define R_AST_BLOCK(index) ((index) << 4)
#define R_AST_IS_BINARY_EXPRESSION(kind) ((kind) >= R_AST__FirstBinaryExpression && (kind) <= R_AST__LastBinaryExpression)

typedef enum R_AST_Kind
{
	R_AST_None                           = 0,

	R_AST_Argument,
	R_AST_Parameter,
	R_AST_ReturnType,

	R_AST__FirstExpression,

	R_AST__FirstPrimaryExpression        = R_AST__FirstExpression,
	R_AST_Identifier                     = R_AST__FirstPrimaryExpression,
	R_AST_String,
	R_AST_Int,
	R_AST_Float,
	R_AST_Bool,
	R_AST_ProcLit,
	R_AST_ProcType,
	R_AST_StructType,
	R_AST_Compound,
	R_AST__LastPrimaryExpression         = R_AST_Compound,

	R_AST__FirstPostfixUnaryExpression,
	R_AST_Subscript                      = R_AST__FirstPostfixUnaryExpression,
	R_AST_Slice,
	R_AST_Member,
	R_AST_Dereference,
	R_AST_Call,
	R_AST__LastPostfixUnaryExpression    = R_AST_Call,

	R_AST__FirstTypePrefixExpression,
	R_AST_PointerType                    = R_AST__FirstTypePrefixExpression,
	R_AST_SliceType,
	R_AST_ArrayType,
	R_AST__LastTypePrefixExpression      = R_AST_ArrayType,

	R_AST__FirstPrefixUnaryExpression,
	R_AST_Pos                            = R_AST__FirstPrefixUnaryExpression,
	R_AST_Neg,
	R_AST_Not,
	R_AST_BitNot,
	R_AST_Reference,
	R_AST__LastPrefixUnaryExpression     = R_AST_Reference,

	R_AST__FirstBinaryExpression         = R_AST_BLOCK(11),
	R_AST__FirstMulLevelBinaryExpression = R_AST__FirstBinaryExpression,
	R_AST_Mul,
	R_AST_Div,
	R_AST_Rem,
	R_AST_BitAnd,
	R_AST_BitShl,
	R_AST_BitShr,
	R_AST_BitSar,
	R_AST__LastMulLevelBinaryExpression  = R_AST_BLOCK(12) - 1,

	R_AST__FirstAddLevelBinaryExpression = R_AST_BLOCK(12),
	R_AST_Add                            = R_AST__FirstAddLevelBinaryExpression,
	R_AST_Sub,
	R_AST_BitOr,
	R_AST_BitXor,
	R_AST__LastAddLevelBinaryExpression  = R_AST_BLOCK(13) - 1,

	R_AST__FirstCmpLevelBinaryExpression = R_AST_BLOCK(13),
	R_AST_IsEqual                        = R_AST__FirstCmpLevelBinaryExpression,
	R_AST_IsNotEqual,
	R_AST_IsLess,
	R_AST_IsLessEq,
	R_AST_IsGreater,
	R_AST_IsGreaterEq,
	R_AST__LastCmpLevelBinaryExpression  = R_AST_BLOCK(14) - 1,

	R_AST__FirstAndLevelBinaryExpression = R_AST_BLOCK(14),
	R_AST_And                            = R_AST__FirstAndLevelBinaryExpression,
	R_AST__LastAndLevelBinaryExpression  = R_AST_BLOCK(15) - 1,

	R_AST__FirstOrLevelBinaryExpression  = R_AST_BLOCK(15),
	R_AST_Or                             = R_AST__FirstOrLevelBinaryExpression,
	R_AST__LastOrLevelBinaryExpression   = R_AST_BLOCK(16) - 1,
	R_AST__LastBinaryExpression          = R_AST__LastOrLevelBinaryExpression,

	R_AST_Conditional,
	R_AST__LateExpression                = R_AST_Conditional,

	R_AST__FirstDeclaration,
	R_AST_Variable                       = R_AST__FirstDeclaration,
	R_AST_Constant,
	R_AST__LastDeclaration               = R_AST_Constant,

	R_AST__FirstStatement,
	R_AST_Block                          = R_AST__FirstStatement,
	R_AST_If,
	R_AST_While,
	R_AST_Continue,
	R_AST_Break,
	R_AST_Return,
	R_AST_Assignment,
	R_AST__LastStatement                 = R_AST_Assignment,
} R_AST_Kind;

#undef R_AST_BLOCK

typedef struct R_Expression R_Expression;
typedef struct R_Declaration R_Declaration;
typedef struct R_Statement R_Statement;
typedef struct R_AST R_AST;

typedef struct R_Argument
{
	R_AST_Kind kind;
	struct R_Argument* next;

	R_Expression* name;
	R_Expression* value;
} R_Argument;

typedef struct R_Parameter
{
	R_AST_Kind kind;
	struct R_Parameter* next;

	R_Expression* names;
	R_Expression* type;
	R_Expression* value;
} R_Parameter;

typedef struct R_Return_Type
{
	R_AST_Kind kind;
	struct R_Return_Type* next;

	R_Expression* names;
	R_Expression* type;
} R_Return_Type;

typedef struct R_Expression_Header
{
	R_AST_Kind kind;
	R_Expression* next;
} R_Expression_Header;

typedef struct R_Declaration_Header
{
	R_AST_Kind kind;
	R_Declaration* next;
} R_Declaration_Header;

typedef struct R_Statement_Header
{
	R_AST_Kind kind;
	R_Statement* next;
} R_Statement_Header;

typedef struct R_AST_Header
{
	R_AST_Kind kind;
	R_AST* next;
} R_AST_Header;

typedef struct R_Identifier_Expression
{
	struct R_Expression_Header;
	R_String string;
} R_Identifier_Expression;

typedef struct R_String_Expression
{
	struct R_Expression_Header;
	R_String string;
} R_String_Expression;

typedef struct R_Int_Expression
{
	struct R_Expression_Header;
	R_u64 value;
} R_Int_Expression;

typedef struct R_Float_Expression
{
	struct R_Expression_Header;
	R_f64 value;
} R_Float_Expression;

typedef struct R_Bool_Expression
{
	struct R_Expression_Header;
	R_bool value;
} R_Bool_Expression;

typedef struct R_Proc_Lit_Expression
{
	struct R_Expression_Header;
	R_Parameter* params;
	R_Return_Type* return_types;
	R_Statement* body;
} R_Proc_Lit_Expression;

typedef struct R_Proc_Type_Expression
{
	struct R_Expression_Header;
	R_Parameter* params;
	R_Return_Type* return_types;
} R_Proc_Type_Expression;

typedef struct R_Struct_Type_Expression
{
	struct R_Expression_Header;
	R_Declaration* members;
} R_Struct_Type_Expression;

typedef struct R_Compound_Expression
{
	struct R_Expression_Header;
	R_Expression* expr;
} R_Compound_Expression;

typedef struct R_Subscript_Expression
{
	struct R_Expression_Header;
	R_Expression* array;
	R_Expression* index;
} R_Subscript_Expression;

typedef struct R_Slice_Expression
{
	struct R_Expression_Header;
	R_Expression* array;
	R_Expression* start;
	R_Expression* past_end;
} R_Slice_Expression;

typedef struct R_Member_Expression
{
	struct R_Expression_Header;
	R_Expression* namespace;
	R_Expression* member;
} R_Member_Expression;

typedef struct R_Call_Expression
{
	struct R_Expression_Header;
	R_Expression* proc;
	R_Argument* args;
} R_Call_Expression;

typedef struct R_Array_Type_Expression
{
	struct R_Expression_Header;
	R_Expression* size;
	R_Expression* type;
} R_Array_Type_Expression;

typedef struct R_Unary_Expression
{
	struct R_Expression_Header;
	R_Expression* operand;
} R_Unary_Expression;

typedef struct R_Binary_Expression
{
	struct R_Expression_Header;
	R_Expression* left;
	R_Expression* right;
} R_Binary_Expression;

typedef struct R_Conditional_Expression
{
	struct R_Expression_Header;
	R_Expression* condition;
	R_Expression* then_expr;
	R_Expression* else_expr;
} R_Conditional_Expression;

typedef struct R_Expression
{
	union
	{
		struct R_Expression_Header;

		R_Identifier_Expression ident_expr;
		R_String_Expression string_expr;
		R_Int_Expression int_expr;
		R_Float_Expression float_expr;
		R_Bool_Expression bool_expr;
		R_Proc_Lit_Expression proc_lit_expr;
		R_Proc_Type_Expression proc_type_expr;
		R_Struct_Type_Expression struct_type_expr;
		R_Compound_Expression compound_expr;
		R_Subscript_Expression subscript_expr;
		R_Slice_Expression slice_expr;
		R_Member_Expression member_expr;
		R_Call_Expression call_expr;
		R_Array_Type_Expression array_type_expr;
		R_Unary_Expression unary_expr;
		R_Binary_Expression binary_expr;
		R_Conditional_Expression conditional_expr;
	};
} R_Expression;

typedef struct R_Variable_Declaration
{
	struct R_Declaration_Header;
	R_Expression* names;
	R_Expression* type;
	R_Expression* values;
	R_bool is_uninitialized;
} R_Variable_Declaration;

typedef struct R_Constant_Declaration
{
	struct R_Declaration_Header;
	R_Expression* names;
	R_Expression* type;
	R_Expression* values;
} R_Constant_Declaration;

typedef struct R_Declaration
{
	union
	{
		struct R_Declaration_Header;

		R_Variable_Declaration var_decl;
		R_Constant_Declaration const_decl;
	};
} R_Declaration;

typedef struct R_Block_Statement
{
	struct R_Statement_Header;
	R_Statement* body;
} R_Block_Statement;

typedef struct R_If_Statement
{
	struct R_Statement_Header;
	R_Expression* condition;
	R_Statement* then_stmnt;
	R_Statement* else_stmnt;
} R_If_Statement;

typedef struct R_While_Statement
{
	struct R_Statement_Header;
	R_Expression* condition;
	R_Statement* body;
} R_While_Statement;

typedef struct R_Jump_Statement
{
	struct R_Statement_Header;
	// TODO:
} R_Jump_Statement;

typedef struct R_Return_Statement
{
	struct R_Statement_Header;
	R_Expression* values;
} R_Return_Statement;

typedef struct R_Assignment_Statement
{
	struct R_Statement_Header;
	R_Expression* lhs;
	R_Expression* rhs;
	R_AST_Kind op;
} R_Assignment_Statement;

typedef struct R_Statement
{
	union
	{
		struct R_Statement_Header;

		R_Block_Statement block_stmnt;
		R_If_Statement if_stmnt;
		R_While_Statement while_stmnt;
		R_Jump_Statement jump_stmnt;
		R_Return_Statement return_stmnt;
		R_Assignment_Statement assign_stmnt;
		R_Declaration declaration;
		R_Expression expression;
	};
} R_Statement;

typedef struct R_AST
{
	union
	{
		struct R_AST_Header;

		R_Argument argument;
		R_Parameter parameter;
		R_Return_Type return_type;
		R_Expression expression;
		R_Declaration declaration;
		R_Statement statement;
	};
} R_AST;
