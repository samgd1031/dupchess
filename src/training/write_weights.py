'''
Converts pytorch .pth model format to a binary file with just the weights and biases
binary file is formatted as follows:

number of layers -- int
then for each layer:
    layer number (0 indexed) -- int
    layer width -- int
    layer height -- int
    weights -- qty width*height floats
    biases -- qty width floats

'''
import torch
import sys
import numpy as np

if __name__ == "__main__":
    model = torch.load("./src/training/weights/RUN1_100EP_ep15.pth")
    
    n_layers = len(model) // 2
    layers = [lyr for lyr in model]

    with open('./src/training/weights/test.bindat', 'wb') as fid:
        fid.write(n_layers.to_bytes(4, "little"))
        
        for ii in range(n_layers):
            fid.write(ii.to_bytes(4, "little"))
            layer_size = list(model[layers[2*ii]].shape)
            fid.write(layer_size[0].to_bytes(4, "little"))
            fid.write(layer_size[1].to_bytes(4, "little"))
            numels = layer_size[0] * layer_size[1]
            
            weights = model[layers[2*ii]].view(1, numels)
            weights = weights.cpu()
            nw = weights.numpy()
            fid.write(nw.tobytes())

            bias = model[layers[2*(ii+1) - 1]]
            bias = bias.cpu()
            nb = bias.numpy()
            fid.write(nb.tobytes())