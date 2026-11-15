import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "match"))
import tag_lint  # noqa: E402


class TagLintTest(unittest.TestCase):
    def run_lint(self, files):
        with tempfile.TemporaryDirectory() as d:
            root = Path(d) / "re" / "match"
            (root / "u").mkdir(parents=True)
            for name, text in files.items():
                (root / "u" / name).write_text(text)
            return tag_lint.main(["--root", str(root), "--quiet"])

    def test_clean(self):
        self.assertEqual(self.run_lint({"a.c": "// FLAGS: /O2\n// MATCH: 0x00401000 FUN_00401000\nint f(void){return 0;}\n",
                                        "b.c": "    // MATCH: MASHED.exe 0x00401010 _g\nint g;\n"}), 0)

    def test_trailing_tag(self):
        self.assertEqual(self.run_lint({"a.c": "int f(void);// MATCH: 0x004e4c30 FUN_004e4c30\nint f(void){return 0;}\n"}), 1)

    def test_untagged_ignored(self):
        self.assertEqual(self.run_lint({"a.c": "int f(void){return 0;}\n"}), 0)

    def test_lint_text_reports_line(self):
        bad = tag_lint.lint_text("x\nvoid h(void); // MATCH: 0x00401234 FUN_00401234\n")
        self.assertEqual(bad, [(2, "0x00401234", "void h(void);")])


if __name__ == "__main__":
    unittest.main()
