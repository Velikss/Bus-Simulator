function calculate(entity, entities)
{
    // Do behaviour logic
    // check if entity and entities received
    if(entity && entities) {
        var radius = 10;
        var force = 10;
        var mass = GetEntityMass(entity);
        println("Entity mass: " + mass);
        var maxspeed = GetEntityMaxSpeed(entity);
        println("Entity maxspeed: " + maxspeed);

        var ME = GetEntityCoordinates(entity);
        var entityList = GetEntityList(entities);
        var steeringforce = [0, 0];

        // Do behaviour logic
        entityList.forEach(calc);

        function calc(ent, index) {
            var coords = GetEntityCoordinates(ent)
            var distSquared = (ME[0] - coords[0]) * (ME[0] - coords[0]) + (ME[1] - coords[1]) * (ME[1] - coords[1]);
            if (distSquared < radius * radius && distSquared > 0) {
                // seperation force
                var x = ME[0] - coords[0];
                var y = ME[1] - coords[1];
                // steering force
                steeringforce[0] += x / distSquared;
                steeringforce[1] += y / distSquared;
            }
        }

        var length = Math.sqrt((steeringforce[0] * steeringforce[0]) + (steeringforce[1] * steeringforce[1]))
        steeringforce[0] = (steeringforce[0] / length) * force;
        steeringforce[1] = (steeringforce[1] / length) * force;

        var velocity = GetEntityVelocity(entity);
        var heading = GetEntityHeading(entity);

        // acceleration
        var accelerationX = steeringforce[0] / mass;
        var accelerationY = steeringforce[1] / mass;
        // velocity
        velocity[0] += accelerationX; // timeElapsed missing
        velocity[1] += accelerationY; // timeElapsed missing

        // Return values (set function that applies the modified values to the entity
        SetEntityVelocity(entity, velocity[0], velocity[1]);


        //TODO
        // truncate by maxspeed
        // update pos with velocity vector
        // velocity * 0.9 so that it slows down over time

    }
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
