import stalib_cpp

class FixOrientationProcessor:
    def __init__(self, degrees: int = 0):
        if degrees % 90 != 0:
            raise ValueError("Degrees must be a multiple of 90.")
        self.degrees = degrees

    def process(self, image):
        """
        Process the image by rotating it.
        :param image: numpy array (H, W, C)
        :return: rotated numpy array
        """
        if self.degrees == 0:
            return image
        return stalib_cpp.fix_orientation.fix_orientation(image, self.degrees)
