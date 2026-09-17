from shapely.geometry import LineString, Point
from shapely import distance as getDistance


class Tack:
    def __init__(self, line):
        self.id = 0
        self.line = line

        # connectFlag = -1 - нет соединения
        # connectFlag = 0 - соединение с line.coords[0]
        # connectFlag = 1 - соединение с line.coords[-1]

        self.connectFlag = -1 
        self.prev = None
        self.next = None
    
    def append(self, line):
        end = Tack(line)
        tackCurrent = self
        i = 1
        while (tackCurrent.next):
            tackCurrent = tackCurrent.next
            i = i + 1
        end.prev = self
        end.id = i
        tackCurrent.next = end


    def get_headTack(self):
        tackCurrent = self
        while tackCurrent.prev:
            tackCurrent = tackCurrent.prev
        return tackCurrent
    
    def get_lastTack(self):
        tackCurrent = self
        while tackCurrent.next:
            tackCurrent = tackCurrent.next
        return tackCurrent
    
    def get_id_mostNear(self, point):
        tackCurrent = self.get_headTack()
        countTack = self.get_lastTack().id + 1
        min_distance = float('inf')
        nearest_point = -1
        min_id = -1
        for i in range(countTack, 0, -1):
            if tackCurrent.connectFlag != -1:
                if i == 1: continue
                tackCurrent = tackCurrent.next
                continue
            distance1 = getDistance(point, Point(tackCurrent.line.coords[0]))
            distance2 = getDistance(point, Point(tackCurrent.line.coords[-1]))
            if distance1 > distance2:
                current_distance = distance2
                current_nearest_point = 1
            else:
                current_distance = distance1
                current_nearest_point = 0

            if current_distance < min_distance:
                min_distance = current_distance
                nearest_point = current_nearest_point
                min_id = tackCurrent.id
            if i == 1: continue
            tackCurrent = tackCurrent.next
        return min_id, min_distance, nearest_point
    
    def get_tackId(self, id):
        tackCurrent = self.get_headTack()
        while tackCurrent.id != id: tackCurrent = tackCurrent.next
        return tackCurrent



    
        
# class Path:
#     def __init__(self, tack):
#         self.start = tack
