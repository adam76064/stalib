import stalib_cpp

class PageSplitter:
    def __init__(self, layout_type="auto", bw_threshold=128):
        self.params = stalib_cpp.page_split.PageSplitParams()
        self.params.layout_type = layout_type
        self.params.bw_threshold = bw_threshold

    def process(self, image, dpi_x=300, dpi_y=300):
        """
        Estimate page layout (single, two pages, etc.)
        :param image: numpy array
        :param dpi_x: Horizontal DPI
        :param dpi_y: Vertical DPI
        :return: PageSplitResult
        """
        return stalib_cpp.page_split.estimate_layout(image, dpi_x, dpi_y, self.params)
