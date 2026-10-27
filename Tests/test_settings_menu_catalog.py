from pathlib import Path
import configparser
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]


class MenuCatalogTests(unittest.TestCase):
    def test_catalog_covers_user_ini_without_upgrade_markers(self):
        config = configparser.ConfigParser()
        config.optionxform = str
        config.read(ROOT / 'Source/DynamicGamedayAtmosphere.ini')
        expected = {(section, key) for section in config.sections() for key in config[section]
                    if section != 'General'}
        source = (ROOT / 'Source/SettingsMenuOptions.inc').read_text()
        pairs = re.findall(r'\{[^\n]*?L"([^"]+)",L"([^"]+)"', source)
        self.assertEqual(len(pairs), len(set(pairs)))
        self.assertEqual(set(pairs), expected)

    def test_numeric_bounds_match_configuration_audit(self):
        audit = (ROOT / 'Tests/ConfigurationOptions.inc').read_text()
        catalog = (ROOT / 'Source/SettingsMenuOptions.inc').read_text()
        expected = {(s, k): tuple(map(float, (d, lo, hi))) for s, k, d, lo, hi in
                    re.findall(r'\{L"([^"]+)",L"([^"]+)",([\d.]+),([\d.]+),([\d.]+),', audit)}
        actual = {}
        for line in catalog.splitlines():
            name = re.search(r'L"([^"]+)",L"([^"]+)"', line)
            values = re.search(r',([\d.]+),([\d.]+),([\d.]+),Kind::(\w+)\}', line)
            if name and values and values[4] != 'Profile':
                actual[name.group(1, 2)] = tuple(map(float, values.group(1, 2, 3)))
        self.assertEqual(actual, expected)


if __name__ == '__main__':
    unittest.main()
