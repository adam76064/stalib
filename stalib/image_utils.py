import numpy as np
from PIL import Image

def pil_to_numpy(image: Image.Image) -> np.ndarray:
    """Convert a PIL Image to a numpy array for STALib."""
    return np.array(image)

def numpy_to_pil(image_array: np.ndarray) -> Image.Image:
    """Convert a STALib numpy array back to a PIL Image."""
    if len(image_array.shape) == 2:
        return Image.fromarray(image_array, mode="L")
    elif len(image_array.shape) == 3 and image_array.shape[2] == 3:
        return Image.fromarray(image_array, mode="RGB")
    elif len(image_array.shape) == 3 and image_array.shape[2] == 4:
        return Image.fromarray(image_array, mode="RGBA")
    else:
        raise ValueError("Unsupported array shape for PIL conversion.")
