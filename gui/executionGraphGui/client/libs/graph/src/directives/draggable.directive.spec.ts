import { DraggableDirective } from './draggable.directive';
import { ElementRef } from '../../../../node_modules/@angular/core';

describe('DraggableDirective', () => {
  it('should create an instance', () => {
    const directive = new DraggableDirective(new ElementRef(null));
    expect(directive).toBeTruthy();
  });
});
