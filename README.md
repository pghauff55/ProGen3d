# ProGen3d
3d Procedural Generator

ProGen3d is a grammar that uses variables and symbolic math objects for calculations.
Based on https://github.com/santipaprika/procedural-buildings and https://github.com/mathSlib-dev/mathSlib .

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






<div align="center">
    <img src="snapshot01.png" width="1200px"</img> 
</div>
