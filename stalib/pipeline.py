from .fix_orientation import FixOrientationProcessor
from .page_split import PageSplitter
from .deskew import DeskewProcessor
from .select_content import ContentSelector
from .page_layout import MarginCalculator
from .output import OutputProcessor

class Pipeline:
    """
    Orchestrates all 6 stages of ScanTailor Advanced processing.
    """
    def __init__(self, dpi=300):
        self.dpi = dpi
        
        self.stage1 = FixOrientationProcessor()
        self.stage2 = PageSplitter()
        self.stage3 = DeskewProcessor()
        self.stage4 = ContentSelector()
        self.stage5 = MarginCalculator()
        self.stage6 = OutputProcessor(dpi=dpi)

    def process(self, image):
        """
        Run the full pipeline on a numpy image array.
        """
        # 1. Fix Orientation
        img1 = self.stage1.process(image)
        
        # 2. Page Split (returns layout info, not an image)
        # Assuming single page for simplicity in default pipeline
        split_info = self.stage2.process(img1, self.dpi, self.dpi)
        
        # 3. Deskew
        img3 = self.stage3.process(img1, self.dpi, self.dpi)
        
        # 4. Select Content
        content_res = self.stage4.process(img3, self.dpi, self.dpi)
        
        # 5. Margins
        # Pass page size (W, H)
        page_size = (img3.shape[1], img3.shape[0])
        margins = self.stage5.process(content_res.content_rect, page_size)
        
        # 6. Output
        out_res = self.stage6.process(img3, content_res.content_rect, content_res.page_rect)
        
        return out_res.primary
