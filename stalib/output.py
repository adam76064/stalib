import stalib_cpp

class OutputProcessor:
    def __init__(self, mode="bw", binarization="otsu", despeckle=1.0, dewarping="off", dpi=300):
        self.params = stalib_cpp.output.OutputParams()
        self.params.mode = mode
        self.params.binarization = binarization
        self.params.despeckle = despeckle
        self.params.dewarping = dewarping
        self.params.dpi = dpi

    def process(self, image, content_rect, page_rect):
        return stalib_cpp.output.generate_output(image, content_rect, page_rect, self.params)
