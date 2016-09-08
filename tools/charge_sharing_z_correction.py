# Shows the correction of z to take into account a charge cloud with a sigma != 0 at the beginning
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d.axes3d import Axes3D
from scipy.special import erf
from itertools import product, combinations

import mpl_toolkits.mplot3d.art3d as art3d
from matplotlib.patches import Circle, PathPatch, Rectangle
from matplotlib.widgets import Slider, Button, RadioButtons


def normal(x, mu=0, A=1):
    sigma = get_sigma(A)
    return A * np.exp(-(x - mu) ** 2 / (2 * sigma ** 2))
  
def get_sigma(length, temperature=330., voltage=80.):
    kb_K_e = 8.6173e-5
    return length * np.sqrt(2 * temperature / voltage * kb_K_e)

mu = 0.
x = np.linspace(-30, 30, 5000)

A0 = 23.72
sigma_0 = 1.
 
A1 = np.linspace(A0, A0 + 500, 1000)
def minimizeme(A1, A0, sigma_0):
    return 2 * get_sigma(A1)**2 * np.log(A1 / A0) - (sigma_0)**2
y = minimizeme(A1, A0, sigma_0)
 

# plt.plot(A1, y)
A_new = A1[(np.abs(y)).argmin()]
print get_sigma(200);
print A_new
y1 = normal(x, mu, A0)
y2 = normal(x, mu, A_new)

# #  
plt.plot(x, y1, linewidth = 2, label='Original')
plt.plot(x, y2, linewidth = 2, label='New')
plt.plot([-sigma_0, sigma_0], [A0, A0], linewidth = 2, label='Start sigma = %1.1f' % sigma_0)
plt.legend(loc=0)
plt.xlabel('Position [um]')
plt.ylabel('Charge density')
plt.grid()
plt.show()
