hits = 0.0
spectrum = 0.0
random = 0.0
cooldown = 0
move = 0.0
show_spectrum = 100
spectrum_pos = 0.0

function update()
    if spectrum >= 1. and cooldown <= 0 and hits <= 3.0 then
        hits = hits + 1.0
        cooldown = 10
    else
        cooldown = cooldown - 1
    end

    if hits == 4.0 and spectrum >= 1. and cooldown <= 0 then
        move = 1.0
    end

    if move == 1.0 then
        spectrum_pos = spectrum_pos + 0.005
    end

    random = math.random()
end