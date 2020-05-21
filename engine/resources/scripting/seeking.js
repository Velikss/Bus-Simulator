function calculate(entity, entities) {
    // Do behaviour logic
    // check if entity and entities received
    if (entity && entities) {
        var maxspeed = GetEntityMaxSpeed(entity);
        var ME = GetEntityCoordinates(entity);
        var velocity = GetEntityVelocity(entity);
        var target = GetEntityTarget(entity);
        var steeringforce = [0, 0];

        // Do behaviour logic
        var desiredVelocity = [0, 0];
        desiredVelocity[0] = target[0] - ME[0];
        desiredVelocity[1] = target[1] - ME[1];
        var length = Math.sqrt((desiredVelocity[0] * desiredVelocity[0]) + (desiredVelocity[1] * desiredVelocity[1]))
        if (length > 0) {
            desiredVelocity[0] = (desiredVelocity[0] / length) * maxspeed;
            desiredVelocity[1] = (desiredVelocity[1] / length) * maxspeed;
        } else {
            desiredVelocity[0] = desiredVelocity[0] * maxspeed;
            desiredVelocity[1] = desiredVelocity[1] * maxspeed;
        }

        steeringforce[0] = desiredVelocity[0] - velocity[0];
        steeringforce[1] = desiredVelocity[1] - velocity[1];

        SetEntitySteeringForce(entity, steeringforce[0], steeringforce[1]);
    }
}