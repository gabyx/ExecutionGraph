import { GraphInfoMessages, sz } from '@messages/index';

export { sz as sz };
export { GraphInfoMessages as Messages };

export abstract class GraphInfoService {

  public abstract async getAllGraphTypeDescriptions(): Promise<GraphInfoMessages.GetAllGraphTypeDescriptionsResponse>;
}


