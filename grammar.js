// oops looking at version2

parens = (...rule) => seq('(', ...rule, ')')

braces = (...rule) => seq('{', ...rule, '}')

brackets = (...rule) => seq('[', ...rule, ']')

// ticked = (...rule) => seq('`', ...rule, '`')


sep = (sep, rule) => optional(seq(rule, repeat(seq(sep, rule))))
sep1 = (sep, rule) => seq(rule, repeat(seq(sep, rule)))



module.exports = grammar({
  name: 'piforall',
  file_types: ["pi"],
  word: $ => $.identifier,
  contra: $ => seq(contra, $.expr),
  extras: $ => [
    $.comment,
    /\p{Zs}/,
    /\n/,
  ],
  externals: $ => [
    $.start,
    $.semi,
    $.end,
    /\n/,
  ],
  rules: {
    source_file: $ => $.moduleImports,
    comment: $ => token(choice(
        seq('--', /.*/),
        // FIXME comments are {- -} and nested, this requires a scanner.
        seq( 
          '{-',
          /([^-]|-+[^}])-/,
          '}'
        )
    )),
    moduleImports: $ => seq(
      'module',
      $.identifier,
      'where',
      $.start,
      optional(seq($.importDef, repeat(seq($.semi, $.importDef)), $.semi)),
      sep($.semi, $._decl),
    ),
    importDef: $ => seq(
      'import',
      $.identifier,
    ),
    _decl: $ => choice($.sigDef, $.valDef),
    sigDef: $ => seq($.identifier, ':', $.expr),
    valDef: $ => seq($.identifier, '=', $.expr),
    // PRINTME, TRUSTME, Refl
    // cribbed from haskell source, this was built with buildExpressionParser
    // see also https://tree-sitter.github.io/tree-sitter/creating-parsers#keyword-extraction
    expr: $ => choice(
      prec.left(4,seq($.expr,'=',$.expr)),
      prec.right(3,seq($.expr,'->',$.expr)),
      prec.right(2,seq($.expr,'*',$.expr)),
      $.term
    ),
    // These were foldl as app...
    // expr: $ => prec.right(repeat1($.term)), //term: $ => prec.right(5, repeat1($._factor)),
    term: $ => prec.left(repeat1($._term)),
    _term: $ => (choice(
      'Type',
      $.lambda,
      $.letPairExp,
      $.letExpr,
      'Refl',
      $.contra,
      'TRUSTME',
      'PRINTME',
      $.bconst,
      $.substExpr,
      $.ifExpr,
      $.sigmaTy,
      $.expProdOrAnnotOrParens,
      $.operator,  // stuff has to come after operators, redo this.
      $.variable,
    )),
    lambda: $ => seq(
      '\\',
      repeat1($.variable),
      '.',
      $.expr,
    ),
    bconst: $ => choice('Bool','True','False','Unit','()'),
    ifExpr: $ => seq('if',$.expr,'then',$.expr,'else',$.term),
    letExpr: $ => seq('let',$.variable,'=',$.expr,'in',$.expr),
    letPairExp: $ => seq('let','(',$.variable,',',$.variable,')','=', $.expr,'in',$.expr),
    // conflict?
    expProdOrAnnotOrParens: $ => seq('(',choice(
      $.colonExpr, $.commaExpr, $.expr
    ),')'),
    colonExpr: $ => seq($.expr, ':', $.expr),
    commaExpr: $ => seq($.expr, ',', $.expr),
    substExpr: $ => seq('subst', $.expr, 'by', $.expr),
    contra: $ => seq('contra', $.expr),
    sigmaTy: $ => seq('{',$.variable,':',$.expr,'|',$.expr,'}'),
    variable: $ => $.identifier,
    operator: $ => /[:!#$%&*+.,/<=>?@\\^|-]+/,
    identifier: $ => /[A-Za-z][\w']*/,
  },
  
});