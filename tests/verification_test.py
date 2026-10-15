"""Protect the compatibility check's handling of identifiers and wire literals."""
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'build'))
from verify import canonical_digest


class CompatibilityDigestTests(unittest.TestCase):
  def test_approved_style_changes(self):
    before = '#pragma once\n#include "Old/Header.h"\nint maxCodeUnits = Value::Success;\n'
    after = ('#ifndef BUBBLEFIGHTER_SERVER_TEST_H_\n#define BUBBLEFIGHTER_SERVER_TEST_H_\n'
             '#include "new/header.h"\n// explanatory comment\n'
             'int max_code_units = Value::kSuccess;\n#endif  // BUBBLEFIGHTER_SERVER_TEST_H_\n')
    self.assertEqual(canonical_digest(before), canonical_digest(after))

  def test_wire_and_schema_literals_are_not_normalized(self):
    for before, after in [('"accountName"', '"account_name"'),
                          ('"SELECT id FROM accounts"', '"SELECT id FROM players"'),
                          ('0x1234', '0x1235'), ('120', '300'), ('value + 1', 'value - 1')]:
      with self.subTest(before=before):
        self.assertNotEqual(canonical_digest(before), canonical_digest(after))

  def test_comment_markers_inside_literals_are_significant(self):
    self.assertNotEqual(canonical_digest('"// original"'), canonical_digest('"// changed"'))
    self.assertNotEqual(canonical_digest('"/* original */"'), canonical_digest('"/* changed */"'))

  def test_identifier_spelling_and_private_suffix_are_significant(self):
    self.assertNotEqual(canonical_digest('member'), canonical_digest('member_'))
    self.assertNotEqual(canonical_digest('account_id'), canonical_digest('character_id'))


if __name__ == '__main__':
  unittest.main()
