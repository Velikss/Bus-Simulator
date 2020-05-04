function calculate(entity, entities)
{
    // Do behaviour logic
    // check if entity and entities received
    if(entity && entities) {
        var radius = 10;
        var force = 10;
        var mass = GetEntityMass(entity);
        var maxspeed = GetEntityMaxSpeed(entity);

        var ME = GetEntityCoordinates(entity);
        var entityList = GetEntityList(entities);
        var velocity = GetEntityVelocity(entity);
        var steeringforce = [0, 0];
        var centerOfMass = [0, 0];
        var neighbourCount = 0;

        // Do behaviour logic
        entityList.forEach(calc);

        function calc(ent, index)
        {
            var coords = GetEntityCoordinates(ent)
            var distSquared = (ME[0] - coords[0]) * (ME[0] - coords[0]) + (ME[1] - coords[1]) * (ME[1] - coords[1]);
            if (distSquared < radius * radius && distSquared > 0)
            {
                centerOfMass[0] += coords[0];
                centerOfMass[1] += coords[1];
                neighbourCount++;
            }
            if(neighbourCount > 0)
            {
                centerOfMass[0] /= neighbourCount;
                centerOfMass[1] /= neighbourCount;
                seek(centerOfMass)
            }
            //TODO return steeringForce
        }

        function seek(target)
        {
            var desiredVelocity = [0, 0];
            desiredVelocity[0] = target[0] - Me[0];
            desiredVelocity[1] = target[1] - Me[1];
            var length = Math.sqrt((desiredVelocity[0] * desiredVelocity[0]) + (desiredVelocity[1] * desiredVelocity[1]))
            desiredVelocity[0] = (desiredVelocity[0] / length) * maxspeed;
            desiredVelocity[1] = (desiredVelocity[0] / length) * maxspeed;
            steeringforce[0] = desiredVelocity[0] - velocity[0];
            steeringforce[1] = desiredVelocity[1] - velocity[1];
        }

    }
}