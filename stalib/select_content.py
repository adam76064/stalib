import stalib_cpp

class ContentSelector:
    def __init__(self, enable_page_detection=True, enable_fine_tuning=True):
        self.params = stalib_cpp.select_content.ContentParams()
        self.params.enable_page_detection = enable_page_detection
        self.params.enable_fine_tuning = enable_fine_tuning

    def process(self, image, dpi_x=300, dpi_y=300):
        self.params.dpi_x = dpi_x
        self.params.dpi_y = dpi_y
        return stalib_cpp.select_content.find_content(image, self.params)
