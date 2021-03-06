from __future__ import print_function
import unittest
import titanlib
import numpy as np
import scipy.spatial


"""Convenient vectors used as inputs"""


class UtilTest(unittest.TestCase):
    def test_1(self):
        """Check that the test doesn't fail"""
        N = 5
        np.random.seed(0)
        lats = [0, 1];
        lons = [0, 0];
        status, x, y, z = titanlib.convert_coordinates(lats, lons)

    def test_coordinate_transformation(self):
        lats = [62.0477405103684]
        lons = [16.9677237169006]
        x, y = titanlib.convert_to_proj(lats, lons, "+proj=lcc +lat_0=63 +lon_0=15 +lat_1=63 +lat_2=63 +no_defs +R=6.371e+06")
        self.assertEqual(len(x), 1)
        self.assertEqual(len(y), 1)
        self.assertAlmostEqual(x[0], 102557.8, 1)
        self.assertAlmostEqual(y[0], -104322, 1)

    def test_interpolate_to_points(self):
        olats = [70.1, 70]
        olons = [10.7, 10]
        ilats, ilons = np.meshgrid(np.linspace(50,80,301), np.linspace(10,11, 101))
        ivalues = ilats + ilons
        ovalues = titanlib.interpolate_to_points(ilats, ilons, ivalues, olats, olons)
        np.testing.assert_almost_equal(list(ovalues), [80.8, 80], 4)

if __name__ == '__main__':
    unittest.main()
