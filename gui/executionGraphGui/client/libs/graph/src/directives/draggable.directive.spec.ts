import { DraggableDirective } from '@eg/graph/src/directives/draggable.directive';
import { ElementRef } from '@angular/core';

describe('DraggableDirective', () => {
  it('should create an instance', () => {
    const directive = new DraggableDirective(new ElementRef(null));
    expect(directive).toBeTruthy();
  });
});
