Basic guidelines for starter kit drums:

  * Drums have one ore more audio signal outlets.
  * Drums have one inlet, accepting a velocity between 0 and 1 that also acts
    trigger.
  * A velocity of 1 should yield a peak amplitude of 1.
  * A velocity of 0 should have no effect.
  * Optionally, the inlet may accept additional messages for setting
    parameters other than pitch or velocity.
  * Synths may accept creation arguments, but they must produce meaningful
    output when instantiated without arguments.

