use raylib::prelude::*;

/// # paper-engine
// asdf

pub struct Particle {
    pub pos: Vector2,
    pub vel: Vector2,
    pub radius: f32,
    collision_damping: f32,
}

pub static GRAVITY: f32 = 9.81;

pub fn get_sign(number: f32) -> f32 {
    if number > 0.0 {
        1.0
    } else if number < 0.0 {
        -1.0
    } else {
        0.0
    }
}

impl Particle {
    pub fn new(start_pos: Vector2, vel: Vector2, radius: f32) -> Self {
        Self {
            pos: start_pos,
            vel,
            radius,
            collision_damping: 0.8,
        }
    }

    pub fn collision(&mut self, dh: &RaylibDrawHandle, bounds: &Rectangle) {
        let bounds_size: Vector2 =
            Vector2::new(bounds.x + bounds.width, bounds.y + bounds.height) -
                Vector2::one() * self.radius;

        if self.pos.x > bounds_size.x {
            self.pos.x = bounds_size.x * get_sign(self.pos.x);
            self.vel.x *= -1.0 * self.collision_damping;
        }

        if self.pos.y > bounds_size.y {
            self.pos.y = bounds_size.y * get_sign(self.pos.y);
            self.vel.y *= -1.0 * self.collision_damping;
        }
    }
}
