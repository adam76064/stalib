import stalib_cpp

class DeskewProcessor:
    def __init__(self, angle_deg=None):
        self.angle_deg = angle_deg

    def find_skew(self, image, dpi_x=300, dpi_y=300):
        """Find the skew angle of the image without applying it."""
        return stalib_cpp.deskew.find_skew(image, dpi_x, dpi_y)

    def process(self, image, dpi_x=300, dpi_y=300):
        """
        Apply deskew to the image.
        """
        angle = self.angle_deg
        if angle is None:
            res = self.find_skew(image, dpi_x, dpi_y)
            angle = res.angle
            
        return stalib_cpp.deskew.apply_deskew(image, angle, dpi_x, dpi_y)
