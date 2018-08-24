import { async, TestBed } from '@angular/core/testing';
import { CqrsModule } from './cqrs.module';

describe('CqrsModule', () => {
  beforeEach(
    async(() => {
      TestBed.configureTestingModule({
        imports: [CqrsModule]
      }).compileComponents();
    })
  );

  it('should create', () => {
    expect(CqrsModule).toBeDefined();
  });
});
