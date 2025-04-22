grammar S_ONNX;

// Lexer
WS
    : [ \r\n\t] -> skip
;

fragment A
    : [aA]
;
fragment B
    : [bB]
;
fragment C
    : [cC]
;
fragment D
    : [dD]
;
fragment E
    : [eE]
;
fragment F
    : [fF]
;
fragment G
    : [gG]
;
fragment H
    : [hH]
;
fragment I
    : [iI]
;
fragment J
    : [jJ]
;
fragment K
    : [kK]
;
fragment L
    : [lL]
;
fragment M
    : [mM]
;
fragment N
    : [nN]
;
fragment O
    : [oO]
;
fragment P
    : [pP]
;
fragment Q
    : [qQ]
;
fragment R
    : [rR]
;
fragment S
    : [sS]
;
fragment T
    : [tT]
;
fragment U
    : [uU]
;
fragment V
    : [vV]
;
fragment W
    : [wW]
;
fragment X
    : [xX]
;
fragment Y
    : [yY]
;
fragment Z
    : [zZ]
;
fragment US
    : '_'
;
MODELPROTO
    : M O D E L P R O T O
;
GRAPH
    : G R A P H
;
NAME
    : N A M E
;
NODE
    : N O D E
;
INPUT
    : I N P U T
;
OUTPUT
    : O U T P U T
;
OP_TYPE
    : O P US T Y P E
;
ATTRIBUTE
    : A T T R I B U T E
;
INITIALIZER
    : I N I T I A L I Z E R
;
DOC_STRING
    : D O C US S T R I N G
;
DOMAIN
    : D O M A I N
;
MODEL_VERSION
    : M O D E L US V E R S I O N
;
PRODUCER_NAME
    : P R O D U C E R US N A M E
;
PRODUCER_VERSION
    : P R O D U C E R US V E R S I O N
;
TYPE
    : T Y P E
;
TENSOR_TYPE
    : T E N S O R US T Y P E
;
IR_VERSION
    : I R US V E R S I O N
;
ELEM_TYPE
    : E L E M US T Y P E
;
SHAPE
    : S H A P E
;
DIM
    : D I M
;
DIMS
    : D I M S
;
RAW_DATA
    : R A W US D A T A
;
OPSET_IMPORT
    : O P S E T US I M P O R T
;
DIM_VALUE
    : D I M US V A L U E
;
DIM_PARAM
    : D I M US P A R A M
;
DATA_TYPE
    : D A T A US T Y P E
;
VERSION
    : V E R S I O N
;
VALUE
    : V A L U E
;
INT
    : I N T
;
FLOAT
    : F L O A T
;
STRING
    : S T R I N G
;
BOOL
    : B O O L
;

LBRACKET
    : '['
;
RBRACKET
    : ']'
;
LBRACE
    : '{'
;
RBRACE
    : '}'
;
COMMA
    : ','
;
ASSIGN
    : '='
;

fragment INTEGER_TYPE_SUFFIX
    : 'l'
    | 'L'
;

INTEGER_LITERAL
    : (
        '0'
        | [1-9] [0-9]*
    ) INTEGER_TYPE_SUFFIX?
;

fragment ESCAPE_SEQUENCE
    : '\\' [btnfr"'\\]
;

STRING_LITERAL
    : '"' (
        ESCAPE_SEQUENCE
        | ~[\\"]
    )* '"'
;

BYTES_LITERAL
    : [0-9A-Fa-f]+ 'b'
;

// Parser
model
    : MODELPROTO LBRACE model_body_def RBRACE
;

model_body_def
    : ir_version_def producer_name_def producer_version_def domain_def model_version_def doc_string_def graph_def
        opset_import_def
;

ir_version_def
    : IR_VERSION ASSIGN INTEGER_LITERAL
;

producer_name_def
    : PRODUCER_NAME ASSIGN STRING_LITERAL
;

producer_version_def
    : PRODUCER_VERSION ASSIGN STRING_LITERAL
;

domain_def
    : DOMAIN ASSIGN STRING_LITERAL
;

model_version_def
    : MODEL_VERSION ASSIGN INTEGER_LITERAL
;

doc_string_def
    : DOC_STRING ASSIGN STRING_LITERAL
;

graph_def
    : GRAPH LBRACE graph_body_def RBRACE
;

graph_body_def
    : name_def node_list input_list output_list initializer_list?
;

name_def
    : NAME ASSIGN STRING_LITERAL
;

node_list
    : (
        NODE LBRACE node_def RBRACE
    )+
;

input_list
    : (
        INPUT LBRACE value_info_def RBRACE
    )+
;

output_list
    : (
        OUTPUT LBRACE value_info_def RBRACE
    )+
;

initializer_list
    : (
        INITIALIZER LBRACE tensor_def RBRACE
    )+
;

node_def
    : op_type_def name_def (
        input_list
        | input_arr
    ) (
        output_list
        | output_arr
    ) attribute_list?
;

op_type_def
    : OP_TYPE ASSIGN STRING_LITERAL
;

input_arr
    : INPUT ASSIGN LBRACKET STRING_LITERAL (
        COMMA STRING_LITERAL
    )* RBRACKET
;

output_arr
    : OUTPUT ASSIGN LBRACKET STRING_LITERAL (
        COMMA STRING_LITERAL
    )* RBRACKET
;

attribute_list
    : (
        ATTRIBUTE LBRACE attribute_def RBRACE
    )+
;

attribute_def
    : name_def value_def
;

value_def
    : VALUE ASSIGN STRING_LITERAL
;

value_info_def
    : name_def type_def
;

type_def
    : TYPE LBRACE tensor_type_def RBRACE
;

tensor_type_def
    : TENSOR_TYPE LBRACE elem_type_def shape_def RBRACE
;

elem_type_def
    : ELEM_TYPE ASSIGN (
        INT
        | FLOAT
        | STRING
        | BOOL
    )
;

shape_def
    : SHAPE LBRACE dim_list RBRACE
;

dim_list
    : (
        DIM LBRACE dim_def RBRACE
    )+
;

dim_def
    : DIM_VALUE ASSIGN INTEGER_LITERAL
    | DIM_PARAM ASSIGN STRING_LITERAL
;

tensor_def
    : name_def data_type_def dims_def raw_data_def
;

data_type_def
    : DATA_TYPE ASSIGN (
        INT
        | FLOAT
        | STRING
        | BOOL
    )
;

dims_def
    : DIMS ASSIGN INTEGER_LITERAL+
;

raw_data_def
    : RAW_DATA ASSIGN BYTES_LITERAL
;

opset_import_def
    : OPSET_IMPORT LBRACE domain_def version_def RBRACE
;

version_def
    : VERSION ASSIGN INTEGER_LITERAL
;