Grammar Definition
==================

This chapter will give some grammar definitions related to Berry. We use
**Extended Backus Normal Form** (EBNF) to define or express grammar. We
did not use strict EBNF grammar to define, but made a lot of
simplifications, but these simplifications will not affect readers’
understanding of the grammar.

The EBNF definition of Berry language grammar is as follows:

.. code::

   (* program define *)
   program = block;
   
   (* block define *)
   block = {statement};
   
   (* statement define *)
   statement = class_stmt | func_stmt | var_stmt | if_stmt | while_stmt |
            for_stmt | break_stmt | return_stmt | expr_stmt | import_stmt |
            try_stmt | throw_stmt | ';';
   if_stmt = 'if' expr block {'elif' expr block} ['else' block] 'end';
   while_stmt = 'while' expr block 'end';
   for_stmt = 'for' ID ':' expr block 'end';
   break_stmt = 'break' | 'continue';
   return_stmt = 'return' [expr];
   
   (* function define statement *)
   func_stmt = 'def' ID func_body;
   func_body = '(' [arg_field {',' arg_field}] ')' block 'end';
   arg_field = ['*'] ID;
   
   (* class define statement *)
   class_stmt = 'class' ID [':' ID] class_block 'end';
   class_block = {'var' ID {',' ID} | 'static' ['var'] ID ['=' expr] {',' ID ['=' expr] } | 'static' func_stmt | func_stmt};
   import_stmt = 'import' (ID (['as' ID] | {',' ID}) | STRING 'as' ID);
   
   (* exceptional handling statement *)
   try_stmt = 'try' block except_block {except_block} 'end';
   except_block = except_stmt block;
   except_stmt = 'except' (expr {',' expr} | '..') ['as' ID [',' ID]];
   throw_stmt = 'raise' expr [',' expr];
   
   (* variable define statement *)
   var_stmt = 'var' ID ['=' expr] {',' ID ['=' expr]};
   
   (* expression define *)
   expr_stmt = expr [assign_op expr];
   expr = suffix_expr | unop expr | expr binop expr | range_expr | cond_expr;
   cond_expr = expr '?' expr ':' expr; (* conditional expression *)
   assign_op = '=' | '+=' | '-=' | '*=' | '/=' |
               '%=' | '&=' | '|=' | '^=' | '<<=' | '>>=';
   binop = '<' | '<=' | '==' | '!=' | '>' | '>=' | '||' | '&&' |
           '<<' | '>>' | '&' | '|' | '^' | '+' | '-' | '*' | '/' | '%';
   range_expr = expr '..' [expr]
   unop = '-' | '!' | '~';
   suffix_expr = primary_expr {call_expr | ('.' ID) | '[' expr ']'};
   primary_expr = '(' expr ')' | simple_expr | list_expr | map_expr | anon_func | lambda_expr;
   simple_expr =  INTEGER | REAL | STRING | ID | 'true' | 'false' | 'nil';
   call_expr = '(' [expr {',' expr}] ')';
   list_expr = '[' {expr ','} [expr] ']';
   map_expr = '{' {expr ':' expr ','} [expr ':' expr] '}';
   anon_func = 'def' func_body;
   
   (* anonymous function *)
   lambda_expr = '/' [arg_field {',' arg_field}] | {arg_field}] '->' expr;

The standard EBNF format can be found in related materials. Here is an
explanation of the details that need attention when reading the above
grammar. The symbols that have appeared to the left of the equal sign
are non-terminal symbols, and the others are terminal symbols. The
terminator enclosed in quotation marks ``’`` is a fixed string, which is
usually a language keyword or operator. There are several terminators
that are inconvenient to describe directly in EBNF: ``INTEGER``
represents the integer face value; ``REAL`` represents the real number
face value; ``STRING`` represents the string literal value; ``ID``
represents the identifier. These terminators can be defined using
regular expressions:

-  ``INTEGER``: ``0x[a-fA-F0-9]+|\d+``.

-  ``REAL``: ``(\d+\.?|\.\d)\d*([eE][+-]?\d+)?``.

-  ``STRING``: ``"(\\.|[^"])*"|’(\\.|[^’])*’``.

-  ``ID``: ``[_a-zA-Z]\w*``

The symbols that appear sequentially in the standard EBNF are separated
by commas. For intuitiveness, I use spaces to implement the comma
function. The vertical bar symbol “\|” is pronounced as “or”, it means
that the left and right patterns can only match one of them, or has the
lowest priority. For example, the grammar *a*\ 0\ *a*\ 1\|\ *a*\ 2 means
either the matching formula *a*\ 0\ *a*\ 1 or the matching *a*\ 2. The
square brackets indicate that the sub-expression inside the parentheses
are matched 0 or 1 times, the curly braces indicate that the internal
sub-expression is matched 0 or more times, and the parentheses only have
the function of taking the internal sub-expression as a whole.

The following is the JSON grammar definition supported by the JSON
module in the Berry standard library. The usage of EBNF still complies
with the above conventions:

.. code::

   json = value;
   value = object | array |
           string | number | 'true' | 'false' | 'null';
   object = '{' [ string ':' value ] { ',' string ':' value } '}';
   array = '[' [json] { ',' json } ']';

Non-terminal symbols ``string`` and ``number`` can also be defined using
regular expressions. http://www.json.org gives the standard grammar of
JSON, which also includes the definitions of ``string`` and ``number``.
The Berry JSON library’s support for numbers is different from the
standard. The standard JSON numbers must start with “-” or the number
“0-9”, while the Berry JSON library also accepts numbers starting with a
decimal point.
