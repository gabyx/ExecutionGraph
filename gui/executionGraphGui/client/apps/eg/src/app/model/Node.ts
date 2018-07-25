export type UIProps = {
  x: number,
  y: number
}

export class Node {
  constructor(
    public readonly id: string, 
    public readonly name: string, 
    public readonly uiProps: UIProps = {x: 0, y: 0})
  {}
}
