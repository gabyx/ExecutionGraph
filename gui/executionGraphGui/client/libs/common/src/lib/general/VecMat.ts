export const epsilon = 0.00001;

export class Vector2 {
  get x(): number {
    return this.values[0];
  }

  get y(): number {
    return this.values[1];
  }

  get xy(): [number, number] {
    return [this.values[0], this.values[1]];
  }

  set x(value: number) {
    this.values[0] = value;
  }

  set y(value: number) {
    this.values[1] = value;
  }

  set xy(values: [number, number]) {
    this.values[0] = values[0];
    this.values[1] = values[1];
  }

  constructor(values?: [number, number]) {
    if (values !== undefined) {
      this.xy = values;
    }
  }

  private values = new Float32Array(2);

  static readonly zero = new Vector2([0, 0]);
  static readonly one = new Vector2([1, 1]);

  at(index: number): number {
    return this.values[index];
  }

  reset(): void {
    this.x = 0;
    this.y = 0;
  }

  copy(dest?: Vector2): Vector2 {
    if (!dest) {
      dest = new Vector2();
    }

    dest.x = this.x;
    dest.y = this.y;

    return dest;
  }

  negate(dest?: Vector2): Vector2 {
    if (!dest) {
      dest = this;
    }

    dest.x = -this.x;
    dest.y = -this.y;

    return dest;
  }

  abs() {
    this.x = Math.abs(this.x);
    this.y = Math.abs(this.y);
    return this;
  }

  equals(vector: Vector2, threshold = epsilon): boolean {
    if (Math.abs(this.x - vector.x) > threshold) {
      return false;
    }

    if (Math.abs(this.y - vector.y) > threshold) {
      return false;
    }

    return true;
  }

  length(): number {
    return Math.sqrt(this.squaredLength());
  }

  squaredLength(): number {
    const x = this.x;
    const y = this.y;

    return x * x + y * y;
  }

  add(vector: Vector2): Vector2 {
    this.x += vector.x;
    this.y += vector.y;

    return this;
  }

  subtract(vector: Vector2): Vector2 {
    this.x -= vector.x;
    this.y -= vector.y;

    return this;
  }

  multiply(vector: Vector2): Vector2 {
    this.x *= vector.x;
    this.y *= vector.y;

    return this;
  }

  divide(vector: Vector2): Vector2 {
    this.x /= vector.x;
    this.y /= vector.y;

    return this;
  }

  scale(value: number): Vector2 {
    this.x *= value;
    this.y *= value;

    return this;
  }

  normalize(dest?: Vector2): Vector2 {
    if (!dest) {
      dest = this;
    }

    let length = this.length();

    if (length === 1) {
      return this;
    }

    if (length === 0) {
      dest.x = 0;
      dest.y = 0;

      return dest;
    }

    length = 1.0 / length;

    dest.x *= length;
    dest.y *= length;

    return dest;
  }

  multiplyMatrix2(matrix: Matrix2, dest?: Vector2): Vector2 {
    if (!dest) {
      dest = this;
    }

    return matrix.multiplyVector2(this, dest);
  }

  static dot(vector: Vector2, vector2: Vector2): number {
    return vector.x * vector2.x + vector.y * vector2.y;
  }

  static distance(vector: Vector2, vector2: Vector2): number {
    return Math.sqrt(this.squaredDistance(vector, vector2));
  }

  static squaredDistance(vector: Vector2, vector2: Vector2): number {
    const x = vector2.x - vector.x;
    const y = vector2.y - vector.y;

    return x * x + y * y;
  }

  static direction(vector: Vector2, vector2: Vector2, dest?: Vector2): Vector2 {
    if (!dest) {
      dest = new Vector2();
    }

    const x = vector.x - vector2.x;
    const y = vector.y - vector2.y;

    let length = Math.sqrt(x * x + y * y);

    if (length === 0) {
      dest.x = 0;
      dest.y = 0;

      return dest;
    }

    length = 1 / length;

    dest.x = x * length;
    dest.y = y * length;

    return dest;
  }

  static mix(vector: Vector2, vector2: Vector2, time: number, dest?: Vector2): Vector2 {
    if (!dest) {
      dest = new Vector2();
    }

    const x = vector.x;
    const y = vector.y;

    const x2 = vector2.x;
    const y2 = vector2.y;

    dest.x = x + time * (x2 - x);
    dest.y = y + time * (y2 - y);

    return dest;
  }

  static sum(vector: Vector2, vector2: Vector2, dest?: Vector2): Vector2 {
    if (!dest) {
      dest = new Vector2();
    }

    dest.x = vector.x + vector2.x;
    dest.y = vector.y + vector2.y;

    return dest;
  }

  static difference(vector: Vector2, vector2: Vector2, dest?: Vector2): Vector2 {
    if (!dest) {
      dest = new Vector2();
    }

    dest.x = vector.x - vector2.x;
    dest.y = vector.y - vector2.y;

    return dest;
  }

  static product(vector: Vector2, vector2: Vector2, dest?: Vector2): Vector2 {
    if (!dest) {
      dest = new Vector2();
    }

    dest.x = vector.x * vector2.x;
    dest.y = vector.y * vector2.y;

    return dest;
  }

  static quotient(vector: Vector2, vector2: Vector2, dest?: Vector2): Vector2 {
    if (!dest) {
      dest = new Vector2();
    }

    dest.x = vector.x / vector2.x;
    dest.y = vector.y / vector2.y;

    return dest;
  }

  static scale(vector: Vector2, value: number, dest?: Vector2): Vector2 {
    if (!dest) {
      dest = new Vector2();
    }

    dest.x = vector.x * value;
    dest.y = vector.y * value;

    return dest;
  }
}

export class Matrix2 {
  constructor(values?: number[]) {
    if (values !== undefined) {
      this.init(values);
    }
  }

  private values = new Float32Array(4);

  static readonly identity = new Matrix2().setIdentity();

  at(index: number): number {
    return this.values[index];
  }

  init(values: number[]): Matrix2 {
    for (let i = 0; i < 4; i++) {
      this.values[i] = values[i];
    }

    return this;
  }

  reset(): void {
    for (let i = 0; i < 4; i++) {
      this.values[i] = 0;
    }
  }

  copy(dest?: Matrix2): Matrix2 {
    if (!dest) {
      dest = new Matrix2();
    }

    for (let i = 0; i < 4; i++) {
      dest.values[i] = this.values[i];
    }

    return dest;
  }

  all(): number[] {
    const data: number[] = [];
    for (let i = 0; i < 4; i++) {
      data[i] = this.values[i];
    }

    return data;
  }

  row(index: number): number[] {
    return [this.values[index * 2 + 0], this.values[index * 2 + 1]];
  }

  col(index: number): number[] {
    return [this.values[index], this.values[index + 2]];
  }

  equals(matrix: Matrix2, threshold = epsilon): boolean {
    for (let i = 0; i < 4; i++) {
      if (Math.abs(this.values[i] - matrix.at(i)) > threshold) {
        return false;
      }
    }

    return true;
  }

  determinant(): number {
    return this.values[0] * this.values[3] - this.values[2] * this.values[1];
  }

  setIdentity(): Matrix2 {
    this.values[0] = 1;
    this.values[1] = 0;
    this.values[2] = 0;
    this.values[3] = 1;

    return this;
  }

  transpose(): Matrix2 {
    const temp = this.values[1];

    this.values[1] = this.values[2];
    this.values[2] = temp;

    return this;
  }

  inverse(): Matrix2 {
    let det = this.determinant();

    if (!det) {
      return null;
    }

    det = 1.0 / det;

    const a11 = this.values[0];

    this.values[0] = det * this.values[3];
    this.values[1] = det * -this.values[1];
    this.values[2] = det * -this.values[2];
    this.values[3] = det * a11;

    return this;
  }

  multiply(matrix: Matrix2): Matrix2 {
    const a11 = this.values[0];
    const a12 = this.values[1];
    const a21 = this.values[2];
    const a22 = this.values[3];

    this.values[0] = a11 * matrix.at(0) + a12 * matrix.at(2);
    this.values[1] = a11 * matrix.at(1) + a12 * matrix.at(3);
    this.values[2] = a21 * matrix.at(0) + a22 * matrix.at(2);
    this.values[3] = a21 * matrix.at(1) + a22 * matrix.at(3);

    return this;
  }

  rotate(angle: number): Matrix2 {
    const a11 = this.values[0];
    const a12 = this.values[1];
    const a21 = this.values[2];
    const a22 = this.values[3];

    const sin = Math.sin(angle);
    const cos = Math.cos(angle);

    this.values[0] = a11 * cos + a12 * sin;
    this.values[1] = a11 * -sin + a12 * cos;
    this.values[2] = a21 * cos + a22 * sin;
    this.values[3] = a21 * -sin + a22 * cos;

    return this;
  }

  multiplyVector2(vector: Vector2, result: Vector2): Vector2 {
    const x = vector.x;
    const y = vector.y;

    if (result) {
      result.xy = [x * this.values[0] + y * this.values[1], x * this.values[2] + y * this.values[3]];

      return result;
    } else {
      return new Vector2([x * this.values[0] + y * this.values[1], x * this.values[2] + y * this.values[3]]);
    }
  }

  scale(vector: Vector2): Matrix2 {
    const a11 = this.values[0];
    const a12 = this.values[1];
    const a21 = this.values[2];
    const a22 = this.values[3];

    const x = vector.x;
    const y = vector.y;

    this.values[0] = a11 * x;
    this.values[1] = a12 * y;
    this.values[2] = a21 * x;
    this.values[3] = a22 * y;

    return this;
  }

  static product(m1: Matrix2, m2: Matrix2, result: Matrix2): Matrix2 {
    const a11 = m1.at(0);
    const a12 = m1.at(1);
    const a21 = m1.at(2);
    const a22 = m1.at(3);

    if (result) {
      result.init([
        a11 * m2.at(0) + a12 * m2.at(2),
        a11 * m2.at(1) + a12 * m2.at(3),
        a21 * m2.at(0) + a22 * m2.at(2),
        a21 * m2.at(1) + a22 * m2.at(3)
      ]);

      return result;
    } else {
      return new Matrix2([
        a11 * m2.at(0) + a12 * m2.at(2),
        a11 * m2.at(1) + a12 * m2.at(3),
        a21 * m2.at(0) + a22 * m2.at(2),
        a21 * m2.at(1) + a22 * m2.at(3)
      ]);
    }
  }
}
