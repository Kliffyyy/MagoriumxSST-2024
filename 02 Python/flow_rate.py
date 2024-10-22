import math

def flow_rate(v, r=None, A=None):

    # Volumetric Flow rate calculation
    '''
    Q = v A
    where:
    Q = Volumetric flow rate (m^3/s)
    v = velocity of the fluid (m/s)
    A = cross-sectional area of the pipe (m^2)
    '''

    # check if there is area, if not calculate the area
    if A:
        return v * A
    return v * (math.pi * r**2)

def volumetric_flow_rate(MFR, density):
    # Volumetric Flow rate calculation
    '''
    Q=ρ/m
    where:
    Q = Volumetric flow rate (m^3/s)
    ρ = Density of the fluid (kg/m^3)
    m = mass flow rate (kg/s)
    '''
    return (MFR / 600) * density

def shear_rate(Q=0, r=0, Y=0):
    # Shear Rate calculation
    '''
    γ = 4Q/πr^3
    where:
    γ = Shear Rate (s^-1)
    Q = Volumetric flow rate (m^3/s)
    r = radius of the pipe (m)
    '''

    '''
    Q = (γ*πr^3)/4
    '''
    # return (4 * Q) / (math.pi * r**3)
    return (Y*math.pi*r**3)/4

def viscosity(F, A, du_dy):
    # calculate viscosity based on shear rate and volumetric flow rate
    '''
    F = µA(du/dy)
    rearranging the equation we get:
    µ = F / A(du/dy)
    where:
    µ = dynamic viscosity of the fluid (Pa.s)
    F = Force (N)
    A = cross-sectional area of the pipe (m^2)
    du/dy = shear rate (s^-1)
    '''

    return F / (A * du_dy)

def calculate_viscosity(volumetric_flow_rate, radius, Force, Area):
    shearRate   = shear_rate(volumetric_flow_rate, radius)
    shearStress = Force / Area
    print(f"shearRate = {shearRate}")
    print(f"shearStress = {shearStress}")
    return shearStress / shearRate #Pa.s

def pressure_difference(Q, L, r, µ):
    # Hagen–Poiseuille equation
    '''
    𝚫P = 8µLQ/ πr^4
    where:
    𝚫P = Pressure Difference (Pa)
    Q = Volumetric flow rate (m^3/s)
    L = length of the pipe (m)
    r = radius of the pipe (m)
    µ = dynamic viscosity of the fluid (Pa.s)
    '''
    return (8 * µ * L * Q) / (math.pi * r**4)

def float_range(start,stop,step):
    istop = int((stop-start) // step)
    for i in range(int(istop)):
        yield round(start + i * step, 5)

class Data:
    def __init__(self, name, MFI, MFR, MFR_RANGE, DENSITY, SHEAR_RATE):
        self.name           = name # material name
        self.MFI            = MFI #°C/kg
        self.MFR            = MFR #g/10min
        self.MFR_RANGE      = list(float_range(MFR_RANGE[0], 
                                               MFR_RANGE[1], 
                                               MFR_RANGE[2]))
        #g/10min  # acceptable range of MFR
        self.DENSITY        = list(float_range(DENSITY[0], 
                                               DENSITY[1], 
                                               DENSITY[2])) 
        #g/cm^3

        self.melt_temperature = MFI.split('/')[0] #°C
        self.SHEAR_RATE = SHEAR_RATE    # g/cc OR Pa*s

# Polypropylene (PP)
PP      = Data(
            'PP', 
            "230/2.16", 
            24.5, 
            (0.2, 50, 0.1), 
            (0.910, 0.928, 0.001), 
            89.0) ###

# High-density polyethylene (HDPE)
HDPE    = Data(
            'HDPE', 
            "190/2.16", 
            8.0, 
            (0.2, 25.0, 0.1), 
            (0.950, 0.960, 0.001), 
            89.0)


if __name__ == '__main__':
    target_plastic = HDPE
    target_velocity             = 200 #mm/s
    target_pressure             = 30,000,000 #Pa 30MPa

    # Input
    ## ARM
    weight                      = 50 #kg
    gravitational_Constant      = 9.81 #m/s^2
    force                       = weight * gravitational_Constant #N 490.5N
    arm_length                  = 0.8 #m

    ## Pipe
    pipe_radius                 = 0.032 #m 32mm
    pipe_length                 = 0.5 #m # pushing distance
    pipe_cross_sectional_area   = math.pi * (pipe_radius**2) #m^2

    # assumption is force generated is always tangent to the arm
    torque                      = force * arm_length #Nm 392.4Nm 
    arm_piston_distance         = 0.21 #m
    piston_force                = torque / arm_piston_distance #N 1962N
    Pressure                    = piston_force/pipe_cross_sectional_area #Pa 30MPa

    # ?????
    velocity                    = 300 #mm/s
    velocity                    = velocity / 1000 #m/s

    # Calculations
    flowRate                    = volumetric_flow_rate(target_plastic.MFR, target_plastic.DENSITY[len(PP.DENSITY)//2]) #m^3/s
    flowRate_cm3_s              = flowRate * 1000000 #cm^3/s

    mu                          = calculate_viscosity(flowRate, 
                                                      pipe_radius, 
                                                      piston_force, 
                                                      pipe_cross_sectional_area) #Pa.s

    PressureDifference          = pressure_difference(flowRate, 
                                                      pipe_length, 
                                                      pipe_radius, 
                                                      mu) #Pa
    PressureDifference_MPa      = PressureDifference / 1000000 #MPa

    # Output
    print(f'Volumetric Flow Rate:           {round(flowRate, 5)} m^3/s ≈ {round(flowRate_cm3_s, 3)} cm^3/s')
    print(f'Pressure Difference:            {round(PressureDifference, 3)} Pa ≈ {round(PressureDifference_MPa, 3)} MPa')
    print(f'Viscosity at {PP.melt_temperature}°C:             {round(mu, 5)} Pa.s')