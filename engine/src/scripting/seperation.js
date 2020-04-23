function calculate(entity, entities)
{
    // check if entities received
    if(entities)
    {
        println("EntityGroup ptr: " + entities);
    }

    // check if entity received
    if(entity)
        println("Entity ptr: " + entity);

    var entityList = GetEntityList(entities);
    println("tesT: " + entityList[0]);

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
