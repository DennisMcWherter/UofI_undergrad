In my software I have a very simple topological architecture. I have a main view and a 
puzzle controller and model. The controller handles all input events (i.e. tells them where
to go) and the model does all processing which is directly related to puzzle solving.

The reason I used this particular method was to wrap all puzzle solving functionality into
the model. Then, if any implementation changes, only adjustments to the model will be
required.

This employs the MVC method since all model-related actions are done directly
through the model, and the view and controller are entirely separate. For example,
the view only handles events related to creating new GUI elements, etc. and the controller
determines how to process particular input.

As for UI, I used the Two-Panel selector. The main panel was used for displaying boards
and solutions while the secondary panel (left-most) was used to update some status
of the current action being performed.