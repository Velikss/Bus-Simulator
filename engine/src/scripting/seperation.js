function calculate(entity, entities)
{
    // Do behaviour logic
    // check if entity and entities received
    if(entity && entities) {
        var radius = 10;
        var force = 10;

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
        // TODO return steeringForce normalized and multiplied by force

        SetEntitySteeringForce(entity, steeringforce[0], steeringforce[1]);
    }
}
