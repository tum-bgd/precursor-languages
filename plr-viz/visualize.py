import json
import sys
import numpy as np
from imageio import imwrite, imread
from skimage.transform import resize, rotate

class cfg:
    tile=(128,128)
gSprites=None;

def get_sprites(path="../plc-emscripten/gui/gfx"):
    sprites= {
        "#": imread("%s/blocked.png" %(path)),
        " ": imread("%s/empty.png"%(path)),
        "X": imread("%s/goal.png"%(path)),
        "*": imread("%s/item.png"%(path)),
        "E": imread("%s/robot.png"%(path)),
        }
    sprites["N"] = rotate(sprites["E"], 90);
    sprites["W"] = rotate(sprites["N"], 90);
    sprites["S"] = rotate(sprites["W"], 90);
    for k in sprites:
        sprites[k] = resize(sprites[k], cfg.tile)
        sprites[k] = sprites[k][:,:,:3] #ignores alpha
    return sprites



def to_matrix(s):
    data = [x for x in s.split("\n") if len(x) != 0];
    rows = len(data);
    cols = len(data[0])
    for row in data:
        print(row)
        if len(row) != cols :
            raise "Not all rows have same column size!"
    # rows and cols known. create matrix
    return np.array([ [y for y in x] for x in data])


def get_row(arr):
    row = np.hstack([gSprites[c] for c in arr])
    return row

def visualize(lno,line):
    line=json.loads(line)
    before = to_matrix(line["arena_before"])    
    image = np.vstack([get_row(row) for row in before])
    imwrite("arena-%04d-0before.png" %(lno),(image*255).astype(np.uint8))
    after = to_matrix(line["arena"])    
    image = np.vstack([get_row(row) for row in after])
    imwrite("arena-%04d-0result.png"%(lno),(image*255).astype(np.uint8))
    
if __name__=="__main__":
    gSprites = get_sprites()
    for s in gSprites:
        print(s,gSprites[s].shape)
    for lno,line in enumerate(open(sys.argv[1],"r")):
        visualize(lno,line)
        print(lno)
