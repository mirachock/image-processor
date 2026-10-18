#!/usr/bin/python3
from typing import cast

from collections import namedtuple
from functools import reduce
from PIL import Image, ImageChops, UnidentifiedImageError
import math
import operator
import os
import subprocess
import sys
import tempfile


def calc_images_distance(image_path1, image_path2):
    with Image.open(image_path1) as image1, Image.open(image_path2) as image2:
        histogram = ImageChops.difference(image1, image2).histogram()

        return math.sqrt(reduce(operator.add, map(lambda h, i: h * (i ** 2), histogram, range(256))) / (
                float(image1.size[0]) * image1.size[1]))


class ImageProcessorTester:
    TestCase = namedtuple("TestCase", ["name", "input", "args", "eps"])

    class TestCaseFailedException(Exception):
        pass

    def __init__(self, image_processor_executable):
        self.image_processor_executable = image_processor_executable

    @staticmethod
    def fail_test_case(input, name, message):
        print("FAIL [{input}_{name}] {message}".format(input=input, name=name, message=message))
        raise ImageProcessorTester.TestCaseFailedException()

    @staticmethod
    def succeed_test_case(input, name, end='\n'):
        print("OK   [{input}_{name}]".format(input=input, name=name), end=end)

    def run_tests(self):
        inputs = "barbie", "lake", "mandrill", "tree", "chess"

        filter_test_cases = {
            "crop": [
                ImageProcessorTester.TestCase(input="lenna", name="crop_1", args=["-crop", "1000", "1000"],
                                              eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="crop_2", args=["-crop", "200", "100"],
                                              eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="crop_3", args=["-crop", "500", "300"],
                                              eps=0.0),
            ],
            "edge": [
                ImageProcessorTester.TestCase(input="lenna", name="edge_1", args=["-edge", "0.0"], eps=1.0),
                ImageProcessorTester.TestCase(input="lenna", name="edge_2", args=["-edge", "0.05"], eps=1.0),
                ImageProcessorTester.TestCase(input="lenna", name="edge_3", args=["-edge", "0.1"], eps=1.0),
                ImageProcessorTester.TestCase(input="lenna", name="edge_4", args=["-edge", "0.3"], eps=1.0),
            ],
            "gs": [
                ImageProcessorTester.TestCase(input="lenna", name="gs", args=["-gs"], eps=1.0),
            ],
            "neg": [
                ImageProcessorTester.TestCase(input="lenna", name="neg", args=["-neg"], eps=1.0),
            ],
            "sharp": [
                ImageProcessorTester.TestCase(input="lenna", name="sharp", args=["-sharp"], eps=1.0),
            ],
            "blur": [
                ImageProcessorTester.TestCase(input="lenna", name="blur_1", args=["-blur", "7.5"], eps=2.0),
                ImageProcessorTester.TestCase(input="lenna", name="blur_2", args=["-blur", "7.5", "-blur", "3"],
                                              eps=2.0),
                ImageProcessorTester.TestCase(input="lenna", name="blur_3", args=["-blur", "3"], eps=2.0),
                ImageProcessorTester.TestCase(input="lenna", name="blur_4", args=["-blur", "0.5"], eps=2.0),
            ],
            "twirl": [
                ImageProcessorTester.TestCase(input="lenna", name="twirl_1",
                                              args=["-twirl", "[182", "273]", "300.00", "3.14"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl_2",
                                              args=["-twirl", "[182", "273]", "300.00", "1.0"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl_3",
                                              args=["-twirl", "[182", "273]", "100.00", "3.14"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl_4",
                                              args=["-twirl", "[182", "182]", "100.00", "3.14"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl_5",
                                              args=["-twirl", "[180", "145]", "140.0", "2.5"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl_6",
                                              args=["-twirl", "[245", "310]", "210.0", "1.8"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl_7",
                                              args=["-twirl", "[180", "450]", "160.0", "3.5"], eps=0.0),
            ],
            "chain": [
                ImageProcessorTester.TestCase(input="lenna", name="chain_1",
                                              args=["-twirl", "[182", "273]", "300.00", "3.14", "-crop", "300", "500"],
                                              eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="chain_2",
                                              args=["-sharp", "-neg"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="chain_3",
                                              args=["-crop", "300", "300", "-edge", "0.0"], eps=0.0),
            ],
        }
        filter_test_error_cases = {
            "crop": [
                ImageProcessorTester.TestCase(input="lenna", name="crop", args=["-crop"],
                                              eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="crop", args=["-crop", "q"],
                                              eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="crop", args=["-crop", "13"],
                                              eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="crop", args=["-crop", "13"],
                                              eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="crop", args=["-crop", "13", "0"],
                                              eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="crop", args=["-crop", "0", "13"],
                                              eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="crop", args=["-crop", "0", "-45"],
                                              eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="crop", args=["-crop", "67", "67", "67"],
                                              eps=0.0),
            ],
            "edge": [
                ImageProcessorTester.TestCase(input="lenna", name="edge", args=["-edge", "-1"], eps=1.0),
                ImageProcessorTester.TestCase(input="lenna", name="edge", args=["-edge", "zero"], eps=1.0),
                ImageProcessorTester.TestCase(input="lenna", name="edge", args=["-edge"], eps=1.0),
                ImageProcessorTester.TestCase(input="lenna", name="edge", args=["-edge", "24", "3253"], eps=1.0),
                ImageProcessorTester.TestCase(input="lenna", name="edge", args=["-edge", "NaN"], eps=1.0),
            ],
            "gs": [
                ImageProcessorTester.TestCase(input="lenna", name="gs", args=["-gs", "dfds"], eps=1.0),
                ImageProcessorTester.TestCase(input="lenna", name="gs", args=["-gs", "2345"], eps=1.0),
            ],
            "neg": [
                ImageProcessorTester.TestCase(input="lenna", name="neg", args=["-neg", "gfhfg"], eps=1.0),
                ImageProcessorTester.TestCase(input="lenna", name="neg", args=["-neg", "56"], eps=1.0),
            ],
            "sharp": [
                ImageProcessorTester.TestCase(input="lenna", name="sharp", args=["-sharp", "fdsdf"], eps=1.0),
                ImageProcessorTester.TestCase(input="lenna", name="sharp", args=["-sharp", "516"], eps=1.0),
            ],
            "blur": [
                ImageProcessorTester.TestCase(input="lenna", name="blur", args=["-blur"], eps=2.0),
                ImageProcessorTester.TestCase(input="lenna", name="blur", args=["-blur", "acrgd"], eps=2.0),
                ImageProcessorTester.TestCase(input="lenna", name="blur", args=["-blur", "3153.rgd"], eps=2.0),
                ImageProcessorTester.TestCase(input="lenna", name="blur", args=["-blur", "0.5", "1.0"], eps=2.0),
            ],
            "twirl": [
                ImageProcessorTester.TestCase(input="lenna", name="twirl",
                                              args=["-twirl", "[182", "273]098", "300.00", "3.14"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl",
                                              args=["-twirl"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl",
                                              args=["-twirl", "", "150]", "300.00", "3.14"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl",
                                              args=["-twirl", "[", "150]", "300.00", "3.14"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl",
                                              args=["-twirl", "[ 3", "150]", "300.00", "3.14"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl",
                                              args=["-twirl", "[150", "", "300.00", "3.14"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl",
                                              args=["-twirl", "[150", "]", "300.00", "3.14"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl",
                                              args=["-twirl", "[150", "150]", "sgsfd", "3.14"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="twirl",
                                              args=["-twirl", "[150", "150]", "300.00", "3.14", "0.0"], eps=0.0),
            ],
        }
        failed_filters = []

        print(
            "Running image_processor tests\nExecutable: {executable}\n".format(
                executable=tester.image_processor_executable))
        for input in inputs:
            for filter_name, test_cases in filter_test_cases.items():
                try:
                    for test_case in test_cases:
                        test_case = test_case._replace(input=input)
                        self.run_test_case(test_case)
                        print()
                except ImageProcessorTester.TestCaseFailedException:
                    failed_filters.append(filter_name)

            print(
                "Running image_processor ERROR tests\nExecutable: {executable}\n".format(
                    executable=tester.image_processor_executable))

            for filter_name, test_cases in filter_test_error_cases.items():
                try:
                    for num, test_case in enumerate(test_cases):
                        test_case = test_case._replace(input=input)
                        self.run_test_error_case(test_case)
                        print(num, end="\n\n")
                except ImageProcessorTester.TestCaseFailedException:
                    failed_filters.append(filter_name)

        if failed_filters:
            print("-----\nFAILED {count} FILTERS: {filters}\n-----".format(
                count=len(failed_filters),
                filters=", ".join(failed_filters)))
            return False
        else:
            print("-----\nALL FILTERS OK\n-----")
            return True

    def run_test_case(self, test_case):
        global input_file, expected_output_file
        try:
            input_file_name = "{input}.bmp".format(input=test_case.input)
            output_file_name = "{input}_{name}.bmp".format(input=test_case.input, name=test_case.name)
            input_file = os.path.join("test_script", "data", input_file_name)
            expected_output_file = os.path.join("test_script", "data", output_file_name)

            with tempfile.NamedTemporaryFile(suffix=".bmp") as output_file:
                subprocess.check_call([self.image_processor_executable, input_file, output_file.name] + test_case.args,
                                      timeout=180)

                images_distance = calc_images_distance(expected_output_file, output_file.name)
                if images_distance > test_case.eps:
                    self.fail_test_case(test_case.input, test_case.name,
                                        "output image differs from expected with rms diff {diff}".format(
                                            diff=images_distance))

            self.succeed_test_case(test_case.input, test_case.name)
        except subprocess.CalledProcessError:
            self.fail_test_case(test_case.input, test_case.name, "image_processor finished with non-zero exit code")
        except subprocess.TimeoutExpired:
            self.fail_test_case(test_case.input, test_case.name, "timeout")
        except FileNotFoundError:
            # subprocess.run([self.image_processor_executable, input_file, expected_output_file] + test_case.args, timeout=180, check=True)
            self.fail_test_case(test_case.input, test_case.name, "output file not found")
        except UnidentifiedImageError:
            self.fail_test_case(test_case.input, test_case.name, "output file is corrupt")

    def run_test_error_case(self, test_case):
        global input_file, expected_output_file
        try:
            input_file_name = "{input}.bmp".format(input=test_case.input)
            output_file_name = "{input}_{name}.bmp".format(input=test_case.input, name=test_case.name)
            input_file = os.path.join("test_script", "data", input_file_name)
            expected_output_file = os.path.join("test_script", "data", output_file_name)

            with tempfile.NamedTemporaryFile(suffix=".bmp") as output_file:
                subprocess.check_call([self.image_processor_executable, input_file, output_file.name] + test_case.args,
                                      timeout=180)

            # self.succeed_test_case(test_case.input, test_case.name)
            self.fail_test_case(test_case.input, test_case.name, "image_processor finished with zero exit code")
        except subprocess.CalledProcessError:
            self.succeed_test_case(test_case.input, test_case.name, end=" ")
            # self.fail_test_case(test_case.input, test_case.name, "image_processor finished with non-zero exit code")
        except subprocess.TimeoutExpired:
            self.fail_test_case(test_case.input, test_case.name, "timeout")
        except FileNotFoundError:
            self.fail_test_case(test_case.input, test_case.name, "output file not found")
        except UnidentifiedImageError:
            self.fail_test_case(test_case.input, test_case.name, "output file is corrupt")


if __name__ == "__main__":
    tester = ImageProcessorTester(image_processor_executable=sys.argv[1])

    if not tester.run_tests():
        sys.exit(1)
