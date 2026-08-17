import stalib_cpp

class MarginCalculator:
    def __init__(self, top=5.0, bottom=5.0, left=10.0, right=10.0, unit="mm", 
                 h_align="CENTER", v_align="CENTER"):
        self.params = stalib_cpp.page_layout.MarginParams()
        self.params.top = top
        self.params.bottom = bottom
        self.params.left = left
        self.params.right = right
        self.params.unit = unit
        
        # Maps string to enum
        h_map = {
            "AUTO": stalib_cpp.page_layout.HorizontalAlignment.AUTO,
            "LEFT": stalib_cpp.page_layout.HorizontalAlignment.LEFT,
            "CENTER": stalib_cpp.page_layout.HorizontalAlignment.CENTER,
            "RIGHT": stalib_cpp.page_layout.HorizontalAlignment.RIGHT
        }
        v_map = {
            "AUTO": stalib_cpp.page_layout.VerticalAlignment.AUTO,
            "TOP": stalib_cpp.page_layout.VerticalAlignment.TOP,
            "CENTER": stalib_cpp.page_layout.VerticalAlignment.CENTER,
            "BOTTOM": stalib_cpp.page_layout.VerticalAlignment.BOTTOM
        }
        
        self.h_align = h_map.get(h_align.upper(), stalib_cpp.page_layout.HorizontalAlignment.CENTER)
        self.v_align = v_map.get(v_align.upper(), stalib_cpp.page_layout.VerticalAlignment.CENTER)

    def process(self, content_rect, page_size):
        """
        Compute margins.
        """
        return stalib_cpp.page_layout.compute_margins(content_rect, page_size, int(self.h_align), int(self.v_align), self.params)
