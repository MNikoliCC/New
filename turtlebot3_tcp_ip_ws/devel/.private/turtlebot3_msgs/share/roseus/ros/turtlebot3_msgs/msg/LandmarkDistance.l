;; Auto-generated. Do not edit!


(when (boundp 'turtlebot3_msgs::LandmarkDistance)
  (if (not (find-package "TURTLEBOT3_MSGS"))
    (make-package "TURTLEBOT3_MSGS"))
  (shadow 'LandmarkDistance (find-package "TURTLEBOT3_MSGS")))
(unless (find-package "TURTLEBOT3_MSGS::LANDMARKDISTANCE")
  (make-package "TURTLEBOT3_MSGS::LANDMARKDISTANCE"))

(in-package "ROS")
;;//! \htmlinclude LandmarkDistance.msg.html


(defclass turtlebot3_msgs::LandmarkDistance
  :super ros::object
  :slots (_name _distance ))

(defmethod turtlebot3_msgs::LandmarkDistance
  (:init
   (&key
    ((:name __name) "")
    ((:distance __distance) 0.0)
    )
   (send-super :init)
   (setq _name (string __name))
   (setq _distance (float __distance))
   self)
  (:name
   (&optional __name)
   (if __name (setq _name __name)) _name)
  (:distance
   (&optional __distance)
   (if __distance (setq _distance __distance)) _distance)
  (:serialization-length
   ()
   (+
    ;; string _name
    4 (length _name)
    ;; float64 _distance
    8
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; string _name
       (write-long (length _name) s) (princ _name s)
     ;; float64 _distance
       (sys::poke _distance (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; string _name
     (let (n) (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4) (setq _name (subseq buf ptr- (+ ptr- n))) (incf ptr- n))
   ;; float64 _distance
     (setq _distance (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;;
   self)
  )

(setf (get turtlebot3_msgs::LandmarkDistance :md5sum-) "e2f8ddf8c9e39a28149179429f5ae342")
(setf (get turtlebot3_msgs::LandmarkDistance :datatype-) "turtlebot3_msgs/LandmarkDistance")
(setf (get turtlebot3_msgs::LandmarkDistance :definition-)
      "string name #Name od the landmark
float64 distance #Distance to the landmark in meters
")



(provide :turtlebot3_msgs/LandmarkDistance "e2f8ddf8c9e39a28149179429f5ae342")


