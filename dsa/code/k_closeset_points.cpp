#include <iostream>
#include <queue>
#include <vector>
class Point {
   private:
    int x_, y_;

   public:
    Point(int x, int y);
    int getDistance() const;
    int getX() const;
    int getY() const;
    void print() const;
};

class Compare {
   public:
    bool operator()(const Point& a, const Point& b) {
        return a.getDistance() > b.getDistance();
    }
};

Point::Point(int x, int y) : x_(x), y_(y) {}

int Point::getDistance() const { return x_ * x_ + y_ * y_; }

int Point::getX() const { return x_; }
int Point::getY() const { return y_; }

void Point::print() const {
    std::cout << "x: " << x_ << " " << "y: " << y_ << "\n";
}

int main() {
    std::vector<Point> points = {Point(1, 3), Point(-2, 2), Point(-2, 4)};
    int k = 2;
    std::priority_queue<Point, std::vector<Point>, Compare>
        pq;  // cp template format for priority queue

    for (auto& p : points) {
        pq.push(p);
    }

    while (k--) {
        Point p = pq.top();
        p.print();
        pq.pop();

        // use p
    }
}