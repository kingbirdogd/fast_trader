class Setting extends React.Component {
  constructor(props) {
    super(props)
    this.handleChange = this.handleChange.bind(this)
    this.state = {}
  }
  
  componentDidMount() {

  }
  
  handleChange() {
    
  }
  
  render() {
    return(
    
<div className="row">
<div className="col-12 col-sm-12">

<table className="table table-striped table-setting">
<thead>
  <tr>
  <th colSpan="4">自動設定</th>
  </tr>
</thead>
<tbody>
  <tr>
  <td width="10%">觸發賣價 <br className="d-sm-none" />(最多價位)</td>
  <td width="10%">限賣價 <br className="d-sm-none" />(最多價位)</td>
  <td width="10%">止損 <br className="d-sm-none" />(最多價位)</td>
  <td width="10%">止盈 <br className="d-sm-none" />(最多價位)</td>
  </tr>
  
  <tr>
  <td> <input type="text" value="0" onChange={this.handleChange} /> </td>
  <td> <input type="text" value="0" onChange={this.handleChange} /> </td>
  <td> <input type="text" value="-3" onChange={this.handleChange} /> </td>
  <td> <input type="text" value="1" onChange={this.handleChange} /> </td>
  </tr>
</tbody>
</table>

</div>
</div>

    )
  }
}
