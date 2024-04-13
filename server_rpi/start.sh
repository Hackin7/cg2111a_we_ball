#!/bin/sh

REPO_PATH="/home/pi/Documents/cg2111a_we_ball/"
ROS_MASTER_URI=192.168.255.16
ROS_CURR_IP=192.168.255.99

# Start a new tmux session
tmux new-session -d -s alex-session

tmux split-window -h 
tmux split-window -v 
tmux select-pane -t 0 
#tmux split-window -v 
tmux select-pane -t 2
tmux split-window -v 

# Rename panes
PANE_TLS=0
PANE_KILL=1
PANE_LIDAR=2
PANE_ROSCORE=3
tmux select-pane -t $PANE_TLS -T tls-server
tmux select-pane -t $PANE_KILL -T killswitch
tmux select-pane -t $PANE_LIDAR -T lidar
tmux select-pane -t $PANE_ROSCORE -T roscore

# navigate to /home/pi35b/cg2111a_labs/W9S2_TLS_Server
tmux send-keys -t $PANE_TLS "clear" C-m
tmux send-keys -t $PANE_TLS "cd "$REPO_PATH"server_rpi/tls-server-lib/" C-m
tmux send-keys -t $PANE_TLS "./tls-alex-server"

# Split the window vertically

# Run KillSwitch/ local run backup
tmux send-keys -t $PANE_KILL "cd "$REPO_PATH"server_rpi/local_run/" C-m
tmux send-keys -t $PANE_KILL "~/die.sh" C-m
tmux send-keys -t $PANE_KILL "~/arise.sh" C-m
tmux send-keys -t $PANE_KILL "clear" C-m
tmux send-keys -t $PANE_KILL "### KILL SWITCH ###################" C-m
tmux send-keys -t $PANE_KILL "~/die.sh"

# Run Lidar launch file
tmux send-keys -t $PANE_LIDAR "clear" C-m
tmux send-keys -t $PANE_LIDAR "source ~/cg2111a/devel/setup.bash" C-m
tmux send-keys -t $PANE_LIDAR "export ROS_MASTER_URI=http://"$ROS_MASTER_URI":11311" C-m
tmux send-keys -t $PANE_LIDAR "export ROS_HOSTNAME="$ROS_CURR_IP C-m
tmux send-keys -t $PANE_LIDAR "roslaunch rplidar_ros rplidar.launch"


# Run Roscore launch file
tmux send-keys -t $PANE_ROSCORE "clear" C-m
tmux send-keys -t $PANE_ROSCORE "source ~/cg2111a/devel/setup.bash" C-m
tmux send-keys -t $PANE_ROSCORE "export ROS_MASTER_URI=http://"$ROS_CURR_IP":11311" C-m
tmux send-keys -t $PANE_ROSCORE "export ROS_HOSTNAME="$ROS_CURR_IP C-m
tmux send-keys -t $PANE_ROSCORE "roscore"

# Select the left pane
tmux select-pane -t 0

######################### Arduino last min Programming
#tmux new-window 
#-t 4 "cd "$REPO_PATH"arduino/"


# Attach to the tmux session
# https://stackoverflow.com/questions/11832199/tmux-set-g-mouse-mode-on-not-scrolling
tmux set-option -g mouse on 
tmux attach-session -t alex-session