import math

def calculate_flow_rate(Y=0, r=0, Q=0):
    ### Shear Rate calculation
    '''
    γ = 4Q/πr^3
    where:
    γ = Shear Rate (s^-1)
    Q = Volumetric flow rate (m^3/s)
    r = radius of the pipe (m)
    '''
    # return (4 * Q) / (math.pi * r**3)

    ### Volumetric Flow rate calculation
    '''
    Q = (γ*πr^3)/4
    '''
    
    return (Y*math.pi*r**3)/4

def calculate_viscosity(F, A, du_dy):
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
            89.0) ### this value is wrong

# High-density polyethylene (HDPE)
HDPE    = Data(
            'HDPE', 
            "190/2.16", 
            8.0, 
            (0.2, 25.0, 0.1), 
            (0.950, 0.960, 0.001), 
            89.0)


if __name__ == '__main__':
    target_plastic              = HDPE
    target_velocity             = 200 #mm/s
    target_pressure             = 30,000,000 #Pa 30MPa

    # Input
    ## ARM
    weight                      = 50 #kg
    gravitational_Constant      = 9.81 #m/s^2
    force                       = weight * gravitational_Constant #N 490.5N
    arm_length                  = 0.8 #m # location where person pushes

    ## Pipe
    pipe_diameter               = 0.0274 #m 27.4mm
    pipe_radius                 = pipe_diameter / 2 #m
    pipe_length                 = 0.51 #m # pushing distance directly proportional to the amount of plastic extruded
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
    flowRate                    = calculate_flow_rate(target_plastic.SHEAR_RATE, pipe_radius) #m^3/s
    flowRate_cm3_s              = flowRate * 1000000 #cm^3/s

    mu                          = calculate_viscosity(piston_force, pipe_cross_sectional_area, target_plastic.SHEAR_RATE) #Pa.s viscosity/shear_rate?

    PressureDifference          = pressure_difference(flowRate, pipe_length, pipe_radius, target_plastic.SHEAR_RATE) #Pa
    PressureDifference_MPa      = PressureDifference / 1000000 #MPa
    PressureDifference_kPa      = PressureDifference / 1000 #kPa

    ## Output
    print("------ ORIGINAL MACHINE ------ \n")

    print(f"target plastic: {str(target_plastic.name)}")
    print(f"force applied: {round(force, 3)} N")

    print("")

    print(f'Volumetric Flow Rate:           {round(flowRate, 5)} m^3/s ≈ {round(flowRate_cm3_s, 3)} cm^3/s')
    print(f'Pressure Difference:            {round(PressureDifference, 3)} Pa ≈ {round(PressureDifference_kPa, 3)} kPa ≈ {round(PressureDifference_MPa, 3)} MPa')
    print(f'Plastic Melt Temperature:       {target_plastic.melt_temperature}°C')
    print(f'Viscosity at {target_plastic.melt_temperature}°C:             {round(mu, 5)} Pa.s ≈ {round(mu*1000, 5)} cP')