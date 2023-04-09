# portal_GL

## Demo Play Video

<div>
  <div style="display:flex">
    <img style="width: 50%" src="./images/gameplay1.gif" />
    <img style="width: 50%" src="./images/gameplay2.gif" />
  </div>
  <div style="display:flex">
    <img style="width: 50%" src="./images/gameplay3.gif" />
    <img style="width: 50%" src="./images/gameplay4.gif" />
  </div>
</div>

## 1. Game Introduction
This game is a game in which the main character escapes the map using a portal gun. There are two portals, Blue and Orange. The two portals are connected, so if you pass through one portal, you can go to the other. Use the portal to avoid obstacles and escape the map.
## 2. Implementation
### 2.1 Title screen
We implement the title screen with a 2D image.
### 2.2 Graphical (no text-only) help with ‘F1’ key
We implemented the graphical help so that a help window appears when we press F1.
### 2.3 Multiple difficulty levels
We implemented three increasingly difficult stages.
### 2.4 3D Shading
We implemented 3D shading with Game Object Structure that has 3D location, 3D up vector, 3D scale, float value theta. We calculate the model matrix and view matrix and projection matrix and send them to the vertex shader.
### 2.5 Resizable window and its constant aspect-ratio viewport
Our game did not implement the function of maintaining the ratio because it was necessary to maintain the full screen and to remove the mouse pointer.
### 2.6 Dynamic 3D camera movement
The 3D camera is implemented to move along with the main character of the game. We get the location, up, forward, alpha from the game object and calculate the new camera position, up and at. It was implemented in the function below. void dangle_canera_to_game_object(Camera* cam, GameAimingObject* game_object)
### 2.7 Hand-drawn 2D images you created on your own
We used the 2D images that we created.
### 2.8 Physics on gravity and acceleration
We implemented the Physics on gravity and acceleration. Each update, calculate the collision with blocks and if the game object is floating in the air, It was implemented to apply gravitational acceleration.
### 2.9 Portal
We implemented a portal that maintains the physical quantity when it passes. We implemented it so that when we pass through the portal, we can have the same physical quantity based on the normal vector directions of the portal