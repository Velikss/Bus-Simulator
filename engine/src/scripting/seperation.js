function calculate(entity, entities)
{
   // check if entities received
    if(entities)
    {

    }

    // check if entity received
    if(entity)
    {
        var mass = GetEntityMass(entity);
        println("Entity mass: " + mass);
        var speed = GetEntityMaxSpeed(entity);
        println("Entity speed: " + speed);

        SetEntityVelocity(entity, 1.22, 3.44);
    }

    // Do behaviour logic

    // Return values (set function that applies the modified values to the entity

}

/*
Vector2D steeringForce = new Vector2D();

for (int i = 0; i < Entities.Count; i++)
{
    double dist = Vector2D.DistanceSquared(ME.Pos, Entities[i].Pos);
    if (dist < Radius * Radius && dist > 0)
    {
        Vector2D separationForce = ME.Pos - Entities[i].Pos;
        steeringForce += separationForce / dist;
    }
}
return steeringForce.Normalize() * SeperationForce;
*/
