# Script to visualize the charge sharing implemented in the digitizer of the simulation.
# The fraction of charge (0..1), deposited in the pixel is plotted. 
# The pixel geometry, initial charge z-position, temperatur and sensor bias can be changed interactively. 

import numpy as np

import matplotlib.pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d.axes3d import Axes3D
from scipy.special import erf
from itertools import product, combinations

import mpl_toolkits.mplot3d.art3d as art3d
from matplotlib.patches import Circle, PathPatch, Rectangle
from matplotlib.widgets import Slider, Button, RadioButtons

def calc_sigma_diffusion(length, voltage=60, temperature=300):
    kb_K_e = 8.6173e-5
    return length * np.sqrt(2 * temperature / voltage * kb_K_e)

def bivariante_normal_pdf(x, y, mu1, mu2, sigma, A=1):
    return A / (2 * np.pi * sigma ** 2) * np.exp(-((x - mu1) ** 2 + (y - mu2) ** 2) / (2 * sigma ** 2))

def bivariante_normal_cdf(x, y, mu1, mu2, sigma, A=1):
    return A / 4. * (erf((x - mu1) / np.sqrt(2 * sigma ** 2)) + 1) * (erf((y - mu2) / np.sqrt(2 * sigma ** 2)) + 1)

def bivariante_normal_cdf_limits(a1, a2, b1, b2, mu1, mu2, sigma):
    return 1 / (4.) * (erf((a2 - mu1) / np.sqrt(2 * sigma ** 2)) - erf((a1 - mu1) / np.sqrt(2 * sigma ** 2))) * (erf((b2 - mu2) / np.sqrt(2 * sigma ** 2)) - erf((b1 - mu2) / np.sqrt(2 * sigma ** 2)))

def get_plot(x_pixel, y_pixel, z, voltage, temperature):
    sigma = calc_sigma_diffusion(z, voltage, temperature)
    return bivariante_normal_cdf_limits(x_pixel / 2., -x_pixel / 2., y_pixel / 2., -y_pixel / 2., x_grid, y_grid, sigma)

def update_plot(value):
    global l
    ax.collections.remove(l)
    x_pixel, y_pixel, z, voltage, temperature = slider_pitch_x.val, slider_pitch_y.val, slider_z.val, slider_voltage.val, slider_temperature.val
    l = ax.plot_wireframe(x_grid, y_grid, get_plot(x_pixel, y_pixel, z, voltage, temperature), label='cdf', color='blue', alpha=0.3)

# PLotting options
x_min, x_max, y_min, y_max, dx, dy = -150, 150, -150, 150, 4, 4

x_p, y_p, z_p = 1., 1., 0
x_pixel, y_pixel = 250., 50.

mu1, mu2, sigma = x_p, y_p, 1.
x = np.arange(x_min, x_max, dx)
y = np.arange(y_min, y_max, dy)

x_grid, y_grid = np.meshgrid(x, y)
z_pdf_grid = bivariante_normal_pdf(x_grid, y_grid, 0, 0, sigma)
z_pdf_grid_2 = bivariante_normal_pdf(x_grid, y_grid, 0, 0, sigma, 2.)
 
fig = plt.figure(figsize=(14, 6))
ax = fig.add_subplot(1, 2, 1, projection='3d')
l = ax.plot_wireframe(x_grid, y_grid, get_plot(x_pixel, y_pixel, 200, 60, 300), label='cdf', color='blue', alpha=0.3)

ax_pitch_x = plt.axes([0.125, 0.08, 0.3, 0.02])
slider_pitch_x = Slider(ax_pitch_x, 'X dimension [um]', 10, 500, valinit=250)

ax_pitch_y = plt.axes([0.125, 0.06, 0.3, 0.02])
slider_pitch_y = Slider(ax_pitch_y, 'Y dimension [um]', 10, 500, valinit=50)

ax_z = plt.axes([0.125, 0.04, 0.3, 0.02])
slider_z = Slider(ax_z, 'Init. charge Z-pos [um]', 0.1, 1000, valinit=200)

ax_voltage = plt.axes([0.125, 0.02, 0.3, 0.02])
slider_voltage = Slider(ax_voltage, 'Bias voltage [V]', 0.1, 1000, valinit=60)

ax_temperature = plt.axes([0.125, 0.00, 0.3, 0.02])
slider_temperature = Slider(ax_temperature, 'Sensor Temperature [K]', 0.1, 400, valinit=300)

slider_pitch_x.on_changed(update_plot)
slider_pitch_y.on_changed(update_plot)
slider_voltage.on_changed(update_plot)
slider_z.on_changed(update_plot)
slider_temperature.on_changed(update_plot)

ax.auto_scale_xyz([x_min, x_max], [x_min, x_max], [0, 1])

plt.show()
