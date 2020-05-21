function calculate(entity, entities) {
    // Do behaviour logic
    // check if entity and entities received
    if (entity && entities) {
        var force = 0.02;
        var maxspeed = GetEntityMaxSpeed(entity);
        var ME = GetEntityCoordinates(entity);
        var velocity = GetEntityVelocity(entity);
        var targetCoords = GetEntityTarget(entity);
        var steeringforce = [0, 0];

        // Do behaviour logic
        seek(targetCoords);

        var length = Math.sqrt((steeringforce[0] * steeringforce[0]) + (steeringforce[1] * steeringforce[1]))
        if (length > 0) {
            steeringforce[0] = (steeringforce[0] / length) * force;
            steeringforce[1] = (steeringforce[1] / length) * force;
        } else {
            steeringforce[0] *= force;
            steeringforce[1] *= force;
        }

        SetEntitySteeringForce(entity, steeringforce[0], steeringforce[1]);

        function seek(target) {
            var desiredVelocity = [0, 0];
            desiredVelocity[0] = target[0] - ME[0];
            desiredVelocity[1] = target[1] - ME[1];
            var length = Math.sqrt((desiredVelocity[0] * desiredVelocity[0]) + (desiredVelocity[1] * desiredVelocity[1]))
            desiredVelocity[0] = (desiredVelocity[0] / length) * maxspeed;
            desiredVelocity[1] = (desiredVelocity[1] / length) * maxspeed;
            steeringforce[0] = desiredVelocity[0] - velocity[0];
            steeringforce[1] = desiredVelocity[1] - velocity[1];
        }
    }
}