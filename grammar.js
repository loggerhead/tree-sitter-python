/**
 * @file Python grammar for tree-sitter - Dict/Expression Only
 * @author Max Brunsfeld <maxbrunsfeld@gmail.com>
 * @license MIT
 */

/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

module.exports = grammar({
  name: 'python',

  extras: $ => [
    $.comment,
    /[\s\f\uFEFF\u2060\u200B]|\r?\n/,
    $.line_continuation,
  ],

  conflicts: $ => [
  ],

  supertypes: $ => [
    $.expression,
  ],

  externals: $ => [
    $.string,
  ],

  inline: $ => [
  ],

  word: $ => $.identifier,

  rules: {
    source_file: $ => $.expression,

    expression: $ => choice(
      $.string,
      $.integer,
      $.float,
      $.true,
      $.false,
      $.none,
      $.list,
      $.dictionary,
      $.set,
      $.tuple,
    ),

    // Literals

    list: $ => seq(
      '[',
      optional($._collection_elements),
      ']',
    ),

    set: $ => seq(
      '{',
      $._collection_elements,
      '}',
    ),

    tuple: $ => seq(
      '(',
      optional($._collection_elements),
      ')',
    ),

    dictionary: $ => seq(
      '{',
      optional(commaSep1(choice($.pair))),
      optional(','),
      '}',
    ),

    pair: $ => seq(
      field('key', $.expression),
      ':',
      field('value', $.expression),
    ),

    _collection_elements: $ => seq(
      commaSep1($.expression),
      optional(','),
    ),

    // String is handled by external scanner
    
    integer: _ => token(choice(
      seq(
        choice('0x', '0X'),
        repeat1(/_?[A-Fa-f0-9]+/),
        optional(/[Ll]/),
      ),
      seq(
        choice('0o', '0O'),
        repeat1(/_?[0-7]+/),
        optional(/[Ll]/),
      ),
      seq(
        choice('0b', '0B'),
        repeat1(/_?[0-1]+/),
        optional(/[Ll]/),
      ),
      seq(
        repeat1(/[0-9]+_?/),
        choice(
          optional(/[Ll]/),
          optional(/[jJ]/),
        ),
      ),
    )),

    float: _ => {
      const digits = repeat1(/[0-9]+_?/);
      const exponent = seq(/[eE][\+-]?/, digits);

      return token(seq(
        choice(
          seq(digits, '.', optional(digits), optional(exponent)),
          seq(optional(digits), '.', digits, optional(exponent)),
          seq(digits, exponent),
        ),
        optional(/[jJ]/),
      ));
    },

    identifier: _ => /[_\p{XID_Start}][_\p{XID_Continue}]*/,

    true: _ => 'True',
    false: _ => 'False',
    none: _ => 'None',

    comment: _ => token(seq('#', /.*/)),

    line_continuation: _ => token(seq('\\', choice(seq(optional('\r'), '\n'), '\0'))),
  },
});

function commaSep1(rule) {
  return sep1(rule, ',');
}

function sep1(rule, separator) {
  return seq(rule, repeat(seq(separator, rule)));
}
