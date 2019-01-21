import { Vector2 } from '@eg/common';
import { NodeId } from 'apps/eg/src/app/model';

export type Force = Vector2;

export class Body {
  private temp: Vector2 = Vector2.zero.copy();
  constructor(public readonly id: NodeId, position: Vector2, public readonly opaqueData: any) {
    this.position = position;
    this.positionEnd = position.copy();
  }
  public massInv: number = 1.0 / 1.0;

  public position: Vector2;
  public positionEnd: Vector2;

  public velocity = Vector2.zero.copy();
  public velocityEnd = Vector2.zero.copy();

  public extForce: Force = Vector2.zero.copy(); // External force from all ForceLaws

  public async integrateToMidPoint(deltaT: number): Promise<void> {
    Vector2.scale(this.velocity, 0.5 * deltaT, this.temp);
    this.position.add(this.temp);
  }

  public async computeEndVelocity(deltaT: number): Promise<void> {
    Vector2.scale(this.extForce, deltaT * this.massInv, this.velocityEnd).add(this.velocity);
  }

  public async integrateToEndPoint(deltaT: number): Promise<void> {
    Vector2.sum(this.velocityEnd, this.velocity, this.positionEnd)
      .scale(0.5 * deltaT)
      .add(this.position);
  }

  public resetForNextIteration() {
    [this.position, this.positionEnd] = [this.positionEnd, this.position]; // Swap Positions;
    [this.velocity, this.velocityEnd] = [this.velocityEnd, this.velocity]; // Swap Positions;
    this.extForce.reset();
  }

  public async hasPositionConverged(relTol: number, absTol: number): Promise<boolean> {
    Vector2.difference(this.positionEnd, this.position, this.temp).abs();
    // Check convergence in both direction instead of taking the norm... (to expensive...)
    return (
      this.temp.x <= relTol * Math.abs(this.position.x) + absTol &&
      this.temp.y <= relTol * Math.abs(this.position.y) + absTol
    );
  }
}
