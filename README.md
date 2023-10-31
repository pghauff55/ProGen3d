# ProGen3d
3d Procedural Generator

ProGen3d is a grammar that uses variables and symbolic math objects for calculations.
Based on https://github.com/santipaprika/procedural-buildings and https://github.com/mathSlib-dev/mathSlib .

Uses glm vector math, SoiL.h texture loading, Imath vector math, and Glut glu opengl.

### Variables

R variable_name ( min max )
---  Sets a variable

Example
```
X -> R Height ( 4.6 10.7 )
```

### Rules

Rule repeat_variable variable_randomise1 variable_randomise2 ... ; probability -> {Start} | {Repeat_Body} | {End} -> Alternate_Rule

Example
```
X -> R Height ( 4 7 ) R Width (10 13) Y

Y 8 Width Height ; 0.8 -> | S ( Width Height Width ) I ( Cube ) | T ( Width+3 0 0 ) -> Empty

Empty ->
```

### Scale

S ( variable variable variable )
S ( numeric numeric numeric )
S ( calculation calculation calculation )

or any combination of
```
S ( Width-1.5 Height/2 Width*2-4.3 ) 
```

### Translate
T ( variable variable variable )
T ( numeric numeric numeric )
T ( calculation calculation calculation )

or any combination of

```
T ( -1.5 Height/2 1.0/Width ) 
```


### Instantiate
I ( Cube )
I ( Sphere )
I ( Cylinder )



### Example
```
X -> R CenterHeight ( 4 10 ) R Xlength ( 6 8 ) R TowerHeight ( 6 14 ) R BaseHeight ( 2 3 ) Y


Y -> [ Floor ] T ( -50 0 -50 ) [  City ]
City ->  Street Street Street Street Street Street
Street ->  T ( 0 0 15 ) StreetBlocks 
Floor -> S ( 120 0.1 120 ) I ( Cube )
StreetBlocks ->  [ AddBuilding ] [ MoreBlocks ] 
MoreBlocks 8 ->    T ( 10 0 0 )  [ AddBuilding ]  



AddBuilding ; 0.6 -> T ( 0 0 3 ) [ Center ]  T ( 0 0 -2 )  [ SectionX  ]

Center 1 TowerHeight->   S ( 2 TowerHeight 2 ) I ( Cube )  Top 
Top ->  T ( 0 1 0 ) [ S ( 1 0.1 1 ) I ( Sphere ) ] T ( 0 -1 0 )  





Base 1 BaseHeight->   S ( Xlength  BaseHeight  4 )  I ( Cube )

BodyX ->  T ( 0 BaseHeight 0 ) S ( Xlength  CenterHeight  2 )  I ( Cube )  

Xend ->   S (  2  CenterHeight 2 ) T ( 1.75 0 0 ) I ( Cylinder ) 


SectionX CenterHeight CenterHeight Xlength ->   [ Base ] [ BodyX ] [ Xend ] | [ WindowsX ] T ( 0 1 0 ) | 
WindowsX Xlength ->  T ( Xlength/2.0-0.8 0 -1 )   | T ( -0.9 0 0 ) [  S ( 0.8 0.95 0.01 ) T ( 0.9 0 0 ) I ( Cube ) ] | 
```

<div align="center">
    <img src="snapshot01.png" width="1200px"</img> 
</div>
