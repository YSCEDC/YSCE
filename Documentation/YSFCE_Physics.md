# YSFlight Community Edition Physics Documentation

## Introduction

This document contains the physics model of the game and describes how different parameters are calculated. YSFCE runs on SI units, but will accept DAT file inputs in Imperial Units and convert them into SI units.

<br>

# YSFlight Atmospheric Model

The YSFlight atmospheric model is based on the 1976 U.S. Standard Atmosphere model from https://www.digitaldutch.com/atmoscalc/table.htm. The table below shows the various properties of the YSFCE atmosphere. For all parameters at intermediate altitudes, one-dimensional linear interpolation is used with altitudes beyond the table using the closest altitude's values.

| Altitude (m) | Density (kg/m3)    | Temperature (K) | Pressure (Pa) | Speed of sound (m/s) | Viscosity (Pa/s)  |
|----------|------------|-------------|----------|----------------|-------------|
| -2000 | 1.47808    | 301.15 | 127774  | 347.886 | 0.0000187630 |
| -1000 | 1.347      | 294.65 | 113929  | 344.111 | 0.0000184434 |
| 0     | 1.225      | 288.15 | 101325  | 340.294 | 0.0000181206 |
| 1000  | 1.11164    | 281.65 | 89874.6 | 336.434 | 0.0000177943 |
| 2000  | 1.00649    | 275.15 | 79495.2 | 332.529 | 0.0000174645 |
| 3000  | 0.909122   | 268.65 | 70108.5 | 328.578 | 0.0000171311 |
| 4000  | 0.819129   | 262.15 | 61640.2 | 324.579 | 0.0000167940 |
| 5000  | 0.736116   | 255.65 | 54019.9 | 320.529 | 0.0000164531 |
| 6000  | 0.659697   | 249.15 | 47181   | 316.428 | 0.0000161084 |
| 7000  | 0.589501   | 242.65 | 41060.7 | 312.274 | 0.0000157596 |
| 8000  | 0.525168   | 236.15 | 35599.8 | 308.063 | 0.0000154068 |
| 9000  | 0.466348   | 229.65 | 30742.5 | 303.793 | 0.0000150498 |
| 10000 | 0.412707   | 223.15 | 26436.3 | 299.463 | 0.0000146884 |
| 10000 | 0.412707   | 223.15 | 26436.3 | 299.463 | 0.0000146884 |
| 12000 | 0.310828   | 216.65 | 19330.4 | 295.07  | 0.0000143226 |
| 14000 | 0.226753   | 216.65 | 14101.8 | 295.07  | 0.0000143226 |
| 16000 | 0.16542    | 216.65 | 10287.5 | 295.07  | 0.0000143226 |
| 18000 | 0.120676   | 216.65 | 7504.84 | 295.07  | 0.0000143226 |
| 20000 | 0.0880349  | 216.65 | 5474.89 | 295.07  | 0.0000143226 |
| 20000 | 0.0880349  | 216.65 | 5474.89 | 295.07  | 0.0000143226 |
| 22000 | 0.0637273  | 218.65 | 3999.79 | 296.428 | 0.0000144357 |
| 24000 | 0.0462674  | 220.65 | 2930.49 | 297.781 | 0.0000145483 |
| 26000 | 0.0336882  | 222.65 | 2153.09 | 299.128 | 0.0000146604 |
| 28000 | 0.0245988  | 224.65 | 1586.29 | 300.468 | 0.0000147722 |
| 30000 | 0.0180119  | 226.65 | 1171.87 | 301.803 | 0.0000148835 |
| 32000 | 0.013225   | 228.65 | 868.019 | 303.131 | 0.0000149945 |
| 34000 | 0.00960889 | 234.25 | 646.122 | 306.821 | 0.0000153029 |
| 36000 | 0.00703441 | 239.85 | 484.317 | 310.467 | 0.0000156082 |




<br>

# Jet Engine Performance

Jet engine performance is a function of DAT variables and altitude. 

<br>

## Jet Engine Thrust

The thrust of the jet engine is calculated based on the DAT file, throttle setting and altitude (in the form of the jet engine thrust efficiency). This equation does not account for thrust reversers or thrust vectoring.

For non-afterburning operations:

$$T = \eta \, \times \, Thr \, \times \, THRMILIT$$

For afterburning operations: 

$$T = \eta \, \times \, THRMILIT \, \times \, (THRAFTBN \, - \, THRMILIT) \, \times \, Thr$$

Where:
- $\eta$ is the Jet Engine Thrust Efficiency
- $Thr$ is the throttle
<br>

## Jet Engine Thrust Efficiency

The Jet Engine Thrust Effiency $\eta$ is a function of altitude. For all altitudes between 0 and 32000m, $\eta$ is a one-dimensional linear interpolation of the table. For all values outside the table, the value of the closest is used.

| Altitude (m) | $\eta$ |
| ------------ | ------ | 
| -2000 | 1 | 
|4000 | 1|
| 12000 | 0.6 | 
| 16000 | 0.3 | 
| 20000 | 0.084991 | 
| 31999 | 0.084991 | 
| 32000 | 0 | 


## Jet Engine Fuel Consumption

The DAT file defines the sea-level fuel consumption at maximum afterburning and non-afterburning throttle settings. 
<br>

### YSFlight Fuel Consumption

For Afterburning operations:

$$\dot{F} = dt \, \times \, FUELABRN$$

For non-Afterburning operations:

$$\dot{F} = dt \, \times \, FUELMILI \, \times \, Thr$$

Where:
- $\dot{F}$ is the change in fuel weight
- $dt$ is the change in time
- $Thr$ is the throttle setting

<br>

### Future YSFCE Fuel Consumption

For Afterburning Operations:

$$\dot{F} = dt \, \times \, FUELABRN \, \times \, \left( \frac{T}{THRAFTBN} \right)$$

For Non-Afterburning Operations:

$$\dot{F} = dt \, \times \, FUELMILI \, \times \, Thr \times \, \left( \frac{T}{THRAFTBN} \right)$$

Where:
- $\dot{F}$ is the change in fuel weight
- $dt$ is the change in time
- $Thr$ is the throttle setting
- $T$ is the thrust being generated by the engine


<br>

# Propeller Engine Performance

The propeller thrust calculation involves air speed for slow and high-speed calculations. Propeller engines use the following DAT files.

<br>

## Slow Speed Thrust

When airspeed is below the PROPVMIN DAT variable setting there is a different equation for calculating how much thrust the engine produces.

$$P = PROPELLR   \times   Thr   \times   \frac{PROPEFCY}{PROPVMIN}$$ 

<br>

$$T = \frac{\rho_{sea}}{\rho_{alt}}   \times   \frac{P}{V}$$

where:
- $P$ = Power
- $T$ = Thrust
- $\rho_{sea}$ = Air Density at sea level (kg/m3)
- $\rho_{alt}$ = Air Density at aircraft's altitude (kg/m3)
- $V$ = Aircraft Velocity (m/s)
- $Thr$ = Throttle setting

<br>

## High Speed Thrust

When airspeed is equal to or above the PROPVMIN DAT variable setting the following thrust equations apply:

Upon loading the aircraft the following property is calculated:

$$PropK = - \left(\frac{PROPELLR   \times   PROPEFCY}{PROPVMIN^2}\right)$$

Which is used in calcualting the thrust of the aircraft

$$T_{Static} = PROPELLR   \times   \frac{PROPEFCY}{PROPVMIN}$$

<br>

$$T_{Moving} = T_{Static} - PropK \left(PROPVMIN  \times  V \right)$$ 

Where:
- $T_{Static}$ Static thrust of propeller engine
- $T_{Moving}$ Thrust of propeller engine when aircraft flying faster than PROPVMIN.
- $V$ = Aircraft Velocity (m/s)
- $PropK$ = Unknown constant, probably a flux with J/s [^1]

[^1]: Need to understand what this means in real world

<br>

# Mach Number

The mach number of the aircraft is calculated by first determining the speed of sound at the aircraft's altitude and then dividing the aircraft's speed by that value:

$$Mach \, = \, \frac{V}{a}$$

Where:
- $V$ is the aircraft's speed
- $a$ is the speed of sound.

<br>

# Gravity

Gravity is treated as a constant throughout the YSFCE atmosphere:

$$g = 9.807$$

<br>

# Lift & Drag Coefficient Calculation

The Lift and Drag Coefficient calculations are performed when the DAT File goes through the auto-calculate process.

## Lift Coeficient Calculation
$$C_{L_0} = \frac{g  \left(WEIGHCLN + WEIGFUEL\right)}{0.5  \rho_{REFACRUS} REFVCRUS^2 WINGAREA}$$

<br>

$$C_{L_{Land}} = \left(\frac{ g   \left(WEIGHCLN + WEIGFUEL \right)}{0.5  \rho_{Sea} V^2 WINGAREA} \right)   \left(\frac{1}{ 1.0 + CLBYFLAP }\right)   \left(\frac{1}{ 1.0 + CLVARGEO }\right)$$

<br>

$$C_{L_{Slope}} = \frac{C_{L_{Land}} - C_{L_0}}{REFAOALD}$$

where:
- $\rho_{REFACRUS}$ = Air Density at cruise altitude
- $g$ = Gravitational Acceleration
- $\rho_{Sea}$ = Air density at sea level
- $C_{L_0}$ = Lift coefficient at zero angle of attack.
- $C_{L_{Land}}$ = Lift coefficient at landing approach angle of attack

<br>

### Lift Coefficient For Helicopters

Note: For helicopters, $C_{L_0}$ and $C_{L_{Slope}}$ are zero.

<br>

## Drag Coefficient Calculation for Fixed-Wing Aircraft

The drag coefficient has a lot of different considerations that need to be calculated before drag coefficients can be calculated.
- $T_{Cruise}$ = Thrust at REFTCRUS, REFVCRUS, and REFACRUS conditions.
- $T_{V_{MAX}}$ = Thrust at REFACRUS, and MAXSPEED and a throttle setting of 1.0 or Max AB.
- $T_{Landing}$ = Thrust at REFVLAND throttle setting, sea level and at REFVLAND speed without afterburner.

$$C_{D_{0}} = \frac{T_{Cruise}}{0.5   \rho_{REFACRUS} V^2 WINGAREA}$$

<br>

$$C_{D_{Land}} = \left(\frac{T_{Landing}}{\left( 0.5   \rho_{Sea}   REFVLAND^2   WINGAREA \right)}\right)   \left(\frac{1}{ 1.0 + CLBYFLAP }\right)   \left(\frac{1}{ 1.0 + CLVARGEO }\right)   \left(\frac{1}{ 1.0 + CDBYGEAR }\right)$$

<br>

$$C_{D_{Const}} = \frac{C_{D_{Land}}   -   C_{D_{0}}}{REFAOALD^2}$$

<br>

$$C_{D_{MAX}} = \frac{T_{V_{MAX}}}{0.5   \rho_{REVACRUS}   MAXSPEED^2   WINGAREA}$$


<br>

## Drag Coefficient Calculation for Helicopters

Need to calculate properties for the propeller engine.

$$PropK = - \left(\frac{PROPELLR   \times   PROPEFCY}{PROPVMIN^2}\right)$$

Then we can begin to calculate the reference thrust data
- $T_{Ref}$ = Thrust at $Thr_{Ref}$, REFVCRUS, and REFACRUS conditions.

$$Thr_{Ref} = MAXSPEED \sin\left(\frac{\pi}{18}\right)$$
<br>

$$T = T_{ref}   \sin \left( \frac{\pi}{18}  \right)$$

From these reference properties, we can calculate the drag conditions

$$C_{D_{0}} = \frac{T}{0.5   \rho_{REVACRUS}   MAXSPEED^2   WINGAREA}$$
<br>

$$C_{D_{0_{MAX}}} = C_{D_{0}}$$

<br>

$$C_{D_{Const}} = 0$$

<br>


# Brakes

All aircraft (fixed-wing and helicopter) have brakes to slow them down.

## Braking Force

The brake force is calculated by:

$$F_B = B   \times  C_B$$

Where:
- $F_B$ is the braking force
- $B$ is the amount of brakes applied
- $C_B$ is the braking constant.

### Braking Constant Calculation

Helicopters have a different braking constant than fixed-wing aircraft:

$$C_{B_{Helo}} = \frac{\left( WEIGHCLN   +   WEIGFUEL \right)  V^2}{2  \times  REFLNRWY}$$

Where:
- $V$ = 30 m/s

For fixed wing aircraft the REFVLAND value is used instead of 30 m/s.

$$C_{B_{Wing}} = \frac{\left( WEIGHCLN   +   WEIGFUEL \right)   REFVLAND^2}{2   \times   REFLNRWY}$$


# DAT Variables

The following table of DAT variables are used during the performance model calculations for YSFCE.

| DAT Variable | Data | Definition | Unit Type |
| ------------ | ---| ---------- | --------------- |
| WEIGHCLN | float | Weight of aircraft without fuel or cargo/weapons | Weight |
| WEIGFUEL | float | Maximum weight of fuel that can be loaded | Weight |
| PROPELLR | float | Power of the engine | Power |
| PROPVMIN | float | Speed where T = P/V becomes valid. Defaults to 30m/s if not supplied. | Speed |
| PROPEFCY | float | The efficiency factor of the propeller. Defaults to 0.7 if not supplied | None | 
| AFTBURNR | Boolean | Indicates if the aircraft has an afterburning engine. Automatically set to FALSE if propeller DAT Variables are in the DAT file. | N/A |
| THRAFTBN | float | The maximum afterburning thrust output of the engine at sea level | Weight |
| THRMILIT | float | The maximum non-afterburning thrust output of the engine at sea level | Weight |
| FUELABRN | float | Amount of fuel burned per second at maximum afterburning thrust setting at sea level | Weight |
| FUELMILI | float | Amount of fuel burned per second at maximum non-afterburning thrust setting at sea level | Weight |
| WINGAREA | float | Wing Area | Area | 
| CLVARGEO | float | $C_L$ Effect due to variable geometry wing | N/A |
| CDVARGEO | float | $C_D$ effect due to variable geometry wing | N/A |
| CLBYFLAP | float | $C_L$ Effect due to flaps | N/A |
| CDBYFLAP | float | $C_D$ effect due to flaps | N/A |
| CDBYGEAR | float | $C_D$ effect due to landing gear | N/A |
| CRITAOAP | float | Positive critical angle of attack | Angle |
| CRITAOAM | float | Negative critical angle of attack | Angle |
| MAXSPEED | float | Maximum Speed | Speed |
| REFVCRUS | float | Cruise Speed | Speed |
| REFACRUS | float | Cruise Altitude | Length |
| REFTCRUS | float | Cruise throttle setting | N/A |
| REFVCRUS | float | Cruise Speed | Speed |
| REFAOALD | float | Landing approach angle of attack | Angle |
| REFTHRLD | float | Throttle setting at landing | N/A |
| REFVLAND | float | Landing Speed | Speed |
| REFLNRWY | float | Runway distance required to stop | Length |



# Units

YSFlight and YSFCE have many different allowable units for different parameters. For each unit type, YSFlight will convert whatever the user inputs into a default unit for all internal calculations.

| Type | Units | YSFlight Unit |
| -- | -- | -- |
| Length | Foot (ft) <br> Meter (m) <br> Inch (in) <br> Centameter (Cm) <br> Statute Mile (sm) <br> Nautical Mile (nm) | Meter (m) |
| Area | Square Inches ($in^2$), <br> Square Meters ($m^2$) | Square Meters ($m^2$) |
| Weight | Kilograms (kg) <br> Pound (lb) <br> Tonne (t) <br>  Newton (N) | Newton (N) |
| Speed | Mach <br>  Kilometers Per Hour (km/h) <br>  Meters per Second (m/s) <br>  Knots (kt) | Meters per Second (m/s) |
| Angle | Degrees (deg) <br> Radians (rad) | Radians (rad) |
| Power | Horsepower (HP) <br>  Jules per Second (J/s) a.k.a. Watts (W) | Watts (W) |


## Unit Conversions

YSFCE uses the following equations to convert units to the default units used for internal calculations

| Type | Unit | Equation | Output|
| - | - | - | - |
| Weight | kg | $kg \times g$ | N |
| Weight | lb | $lb \times0.453597 \times g$ | N|
| Weight | t | $t \times1000 \times g$ | N |
| Speed | Mach | $340 \times Mach$ | m/s |
| Speed | km/h | $\frac{1000}{3600} \times km/h$ | m/s |
| Speed | kt | $\frac{1852}{3600} \times kt$ | m/s |
| Angle | deg | $\frac{\pi}{180} \times deg$ | radians |
| Area | $in^2$ | $0.000645 \times in^2$| $m^2$ |
| Length | in | $0.0254 \times in$ | m |
| Length | ft | $0.3048 \times ft$ | m |
| Length | km | $1000 \times km$ | m |
| Length | Cm | $100 \times Cm$ | m |
| Length | sm | $1609.0 \times sm$ | m |
| Length | nm | $1852 \times nm$ | m |
| Power | HP | $740 \times HP$ | W |





