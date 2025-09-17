#include "Camera.hpp"

namespace gps {

    // Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;


        // Calculează direcția camerei pe baza poziției și țintei
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
    }

    // Returnează matricea de vizualizare, folosind funcția glm::lookAt()
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, this->cameraUpDirection);
    }
    glm::vec3 Camera::getPosition() {
        return cameraPosition;
    }
    void Camera::setPosition(glm::vec3 newPosition) {
        cameraPosition = newPosition;
    }


    // Actualizează parametrii camerei în urma unui eveniment de mișcare
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        if (direction == MOVE_FORWARD) {
            cameraPosition += speed * cameraFrontDirection;
        }
        if (direction == MOVE_BACKWARD) {
            cameraPosition -= speed * cameraFrontDirection;
        }
        if (direction == MOVE_LEFT) {
            cameraPosition -= glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection)) * speed;
        }
        if (direction == MOVE_RIGHT) {
            cameraPosition += glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection)) * speed;
        }
        // Mișcare pe verticală (sus/jos)
        if (direction == MOVE_UP) {
            cameraPosition += cameraUpDirection * speed;
        }
        if (direction == MOVE_DOWN) {
            cameraPosition -= cameraUpDirection * speed;
        }
    }




    // Actualizează parametrii camerei în urma unui eveniment de rotație
    // yaw - rotația camerei în jurul axei Y
    // pitch - rotația camerei în jurul axei X
    void Camera::rotate(float pitchOffset, float yawOffset) {
        // Actualizează unghiurile
        currentYaw += yawOffset;
        currentPitch += pitchOffset;

        // Constrânge pitch-ul pentru a evita "gimbal lock"
        if (currentPitch > 89.0f) currentPitch = 89.0f;
        if (currentPitch < -89.0f) currentPitch = -89.0f;

        // Calculează noua direcție a camerei
        glm::vec3 front;
        front.x = cos(glm::radians(currentYaw)) * cos(glm::radians(currentPitch));
        front.y = sin(glm::radians(currentPitch));
        front.z = sin(glm::radians(currentYaw)) * cos(glm::radians(currentPitch));
        cameraFrontDirection = glm::normalize(front);
    }


}