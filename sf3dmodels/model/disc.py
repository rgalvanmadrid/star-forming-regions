"""
Disc models collection
======================
Classes: Transition, (missing: Keplerian --> Burgers, Pringle+1981, Keto+2010)
"""

from ..utils.units import au
import numpy as np
"""
class TransitionDiscFunctions(object):
    pass

class PringleDiscFunctions(object):
    #Default: Pringle disc? --> similar to the filaments approach
    #Hamburgers
    #Keto's
    #Transition discs --> class with its own functions
    #
    pass

class HamburgerDiscFunctions(object):
    pass

class DiscModel(DefaultDiscFunctions, HamburgerFunctions):
    #def pringle
    #def default = pringle
    #def hamburger
    pass
"""


#****************
#TRANSITION DISCS
#****************
class Transition(object):
    """
    Host class for transition disc models.
    Included: van Dishoeck+2015
    """
    #Missing: include default models, e.g vandishoeck2015 = {'dens': Transition.func1, 'temp': Transition.func2} 
    
    def __init__(self):
        self.flags = {'disc': True, 'env': False}
        func_scalar = {'powerlaw_cavity': self._powerlaw_cavity_scalar}

    def constant_profile(self, x): return x
    def gaussian_profile(self, x, x_mean, stddev):
        return np.exp(-0.5*((x-x_mean)/stddev)**2)

    def _powerlaw_cavity_scalar(self, 
                                n_cav=1e16, power=-1.0, dn_cav=1e-4,
                                R_cav=30*au, #Radial cavity, must be > 0 
                                z_mean=0, z_stddev=5*au, #Gaussian mean and standard deviation for the disc scale-height
                                phi_mean=0, phi_stddev=None,
                                grid=None, coord=None, func_scalar=False):
        R = coord['R']
        z = coord['z']
        a_cav = n_cav
        if R < R_cav: a_cav *= dn_cav 
        if phi_stddev is not None: 
            phi = coord['phi'] - phi_mean
            phi = (phi>np.pi)*(phi-2*np.pi) + (phi<=np.pi)*phi #Making the grid symmetric with respect to the gaussian val phi_mean
            phi_val = self.gaussian_profile(phi, 0, phi_stddev)
        else: phi_val = 1.0
        val = a_cav*(R/R_cav)**power * self.gaussian_profile(z, z_mean, z_stddev) * phi_val
        return val

    def powerlaw_cavity(self, 
                        n_cav=1e16, power=-1.0, dn_cav=1e-4,
                        R_cav=30*au, #Radial cavity, must be > 0 
                        z_mean=0, z_stddev=5*au, #Gaussian mean and standard deviation for the disc scale-height
                        phi_mean=0, phi_stddev=None,
                        grid=None, coord=None, func_scalar=False):
        if func_scalar: return self._powerlaw_cavity_scalar #If the coord input is scalar
        if coord is not None:
            R = np.asarray(coord['R'])
            z = np.asarray(coord['z'])
            if phi_stddev is not None: 
                phi = np.asarray(coord['phi']) - phi_mean
                phi = np.where(phi > np.pi, phi-2*np.pi, phi)
                phi_val = self.gaussian_profile(phi, 0, phi_stddev)
            else: phi_val = 1.0
            val = n_cav*(R/R_cav)**power * self.gaussian_profile(z, z_mean, z_stddev) * phi_val
            #val = np.zeros(R.shape)
            #cav = R < R_cav
            #val[cav] *= dn_cav
            val = np.where(R < R_cav, dn_cav*val, val)
        if grid is not None:
            profile = (grid.rRTP[1]/R_cav)**power
            if phi_stddev is not None: 
                phi = grid.rRTP[3] - phi_mean
                phi = np.where(phi > np.pi, phi-2*np.pi, phi)
                phi_val = self.gaussian_profile(phi, 0, phi_stddev)
            else: phi_val = 1.0
            val = np.where(grid.rRTP[1] > R_cav, n_cav*profile, dn_cav*n_cav*profile) * self.gaussian_profile(grid.XYZ[2], z_mean, z_stddev) * phi_val
        return val

    
