function calculate(entity, entities)
{
    // Do behaviour logic
    // check if entity and entities received
    if(entity && entities) {
        var radius = 40;
        var force = 0.002;
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
            var length = Math.sqrt((steeringforce[0] * steeringforce[0]) + (steeringforce[1] * steeringforce[1]))
            if(length > 0){
                steeringforce[0] = (steeringforce[0] / length) * force;
                steeringforce[1] = (steeringforce[1] / length) * force;
            }else{
                steeringforce[0] *= force;
                steeringforce[1] *= force;
            }

            SetEntitySteeringForce(entity, steeringforce[0], steeringforce[1]);
        }

        function seek(target)
        {
            var desiredVelocity = [0, 0];
            desiredVelocity[0] = target[0] - ME[0];
            desiredVelocity[1] = target[1] - ME[1];
            var length = Math.sqrt((desiredVelocity[0] * desiredVelocity[0]) + (desiredVelocity[1] * desiredVelocity[1]))
            if(length > 0){
                desiredVelocity[0] = (desiredVelocity[0] / length) * maxspeed;
                desiredVelocity[1] = (desiredVelocity[1] / length) * maxspeed;
            }else{
                desiredVelocity[0] = (desiredVelocity[0]) * maxspeed;
                desiredVelocity[1] = (desiredVelocity[1]) * maxspeed;
            }
            steeringforce[0] = desiredVelocity[0] - velocity[0];
            steeringforce[1] = desiredVelocity[1] - velocity[1];
        }
    }
}